#include "BookingService.h"
#include "Cinema.h"
#include "Customer.h"
#include "Movie.h"
#include "Screen.h"
#include "Show.h"
#include "DBConnection.h"
#include "Logger.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <thread>    // OS Concept: parallel loading using threads
#include <future>    // OS Concept: async result collection
#include <csignal>   // OS Concept: signal handling (SIGINT)
#include <mysql/mysql.h>

using namespace std;

// ─────────────────────────────────────────────
//  OS Concept: SIGINT (Ctrl+C) handler
//  Ensures graceful shutdown — closes DB
//  connection and flushes log before exit.
// ─────────────────────────────────────────────
void handleSigInt(int sig) {
    cout << "\n\n[SIGNAL] Caught SIGINT (signal " << sig << "). Shutting down gracefully...\n";
    Logger::getInstance()->log("SYSTEM", "SIGINT received — closing DB and flushing logs.");
    Logger::getInstance()->flush();
    DBConnection::closeConnection();
    exit(0);
}

// ─────────────────────────────────────────────
//  OS Concept: Thread task — load movies from DB
//  Each thread creates its own MySQL connection
//  (MySQL C API is NOT thread-safe on a shared conn)
// ─────────────────────────────────────────────
vector<Movie> loadMoviesFromDB() {
    vector<Movie> movies;

    // Each thread gets its own connection
    MYSQL* conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, "localhost", "root", "", "cinema_booking", 3306, nullptr, 0)) {
        Logger::getInstance()->log("ERROR", string("loadMoviesFromDB connect: ") + mysql_error(conn));
        mysql_close(conn);
        return movies;
    }

    const char* q = "SELECT title, language, duration_mins FROM movies ORDER BY movie_id";
    if (mysql_query(conn, q)) {
        Logger::getInstance()->log("ERROR", string("loadMoviesFromDB query: ") + mysql_error(conn));
        mysql_close(conn);
        return movies;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) { mysql_close(conn); return movies; }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        movies.emplace_back(row[0], row[1], stoi(row[2]));
    }
    mysql_free_result(res);
    mysql_close(conn);
    Logger::getInstance()->log("INFO", "Movies loaded from DB: " + to_string(movies.size()));
    return movies;
}

// ─────────────────────────────────────────────
//  OS Concept: Thread task — load shows from DB
//  Each thread creates its own MySQL connection
// ─────────────────────────────────────────────
struct ShowData {
    int showId;
    int startTime;
    int screenNumber;
    string movieTitle;
};

vector<ShowData> loadShowDataFromDB() {
    vector<ShowData> showData;

    MYSQL* conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, "localhost", "root", "", "cinema_booking", 3306, nullptr, 0)) {
        Logger::getInstance()->log("ERROR", string("loadShowDataFromDB connect: ") + mysql_error(conn));
        mysql_close(conn);
        return showData;
    }

    const char* q =
        "SELECT s.show_id, s.start_time, sc.screen_number, m.title "
        "FROM shows s "
        "JOIN movies m  ON s.movie_id  = m.movie_id "
        "JOIN screens sc ON s.screen_id = sc.screen_id "
        "ORDER BY s.show_id";

    if (mysql_query(conn, q)) {
        Logger::getInstance()->log("ERROR", string("loadShowDataFromDB query: ") + mysql_error(conn));
        mysql_close(conn);
        return showData;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) { mysql_close(conn); return showData; }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        ShowData sd;
        sd.showId       = stoi(row[0]);
        sd.startTime    = stoi(row[1]);
        sd.screenNumber = stoi(row[2]);
        sd.movieTitle   = row[3];
        showData.push_back(sd);
    }
    mysql_free_result(res);
    mysql_close(conn);
    Logger::getInstance()->log("INFO", "Shows loaded from DB: " + to_string(showData.size()));
    return showData;
}

// ─────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────
int main() {
    // Register SIGINT handler (OS Concept: signal handling)
    signal(SIGINT, handleSigInt);

    cout << "========================================" << endl;
    cout << "      CINEMA TICKET BOOKING SYSTEM"      << endl;
    cout << "========================================\n" << endl;

    // Establish DB connection (singleton)
    try {
        DBConnection::getInstance();
    } catch (const exception& e) {
        cerr << e.what() << "\nExiting.\n";
        return 1;
    }

    // ── OS Concept: Launch two threads concurrently to load data ──────────
    // Thread 1: load movies from DB
    // Thread 2: load show metadata from DB
    // Both run in parallel — reduces startup latency
    auto futureMovies    = async(launch::async, loadMoviesFromDB);
    auto futureShowData  = async(launch::async, loadShowDataFromDB);

    // Wait for both threads to finish (join)
    vector<Movie>    dbMovies   = futureMovies.get();
    vector<ShowData> dbShowData = futureShowData.get();
    // ──────────────────────────────────────────────────────────────────────

    // Build Cinema — loads screens from DB inside constructor
    Cinema PVR("Pacific");

    // Match DB movies by title
    auto findMovie = [&](const string& title) -> Movie* {
        for (auto &m : dbMovies) {
            if (m.getTitle() == title) return &m;
        }
        return nullptr;
    };

    // Build Show objects from DB data
    vector<Show> allShows;
    for (auto &sd : dbShowData) {
        Screen *screen = PVR.getScreen(sd.screenNumber);
        Movie  *movie  = findMovie(sd.movieTitle);
        if (screen && movie) {
            Show show;
            show.setShow(sd.startTime, screen, movie, sd.showId);
            allShows.push_back(show);
        } else {
            Logger::getInstance()->log("WARN",
                "Skipping show — screen or movie not found: " + sd.movieTitle);
        }
    }

    // Fallback: seed with defaults if DB is empty
    if (dbMovies.empty() || allShows.empty()) {
        Logger::getInstance()->log("WARN", "DB empty — using hardcoded seed data.");
        dbMovies.emplace_back("Interstellar", "English", 3);
        dbMovies.emplace_back("Swapped",      "English", 2);
        dbMovies.emplace_back("Inception",    "English", 3);

        Screen *s1 = PVR.getScreen(1);
        Screen *s2 = PVR.getScreen(2);
        Show show1, show2, show3, show4;
        show1.setShow(9,  s1, &dbMovies[0], 1);
        show2.setShow(10, s2, &dbMovies[1], 2);
        show3.setShow(2,  s1, &dbMovies[2], 3);
        show4.setShow(1,  s2, &dbMovies[1], 4);
        allShows = {show1, show2, show3, show4};
    }

    BookingService orchestrator;
    orchestrator.initializeSystem(&PVR, allShows);

    // Register / Authenticate customer
    cout << "Enter phone number: ";
    long long phone = 0;
    if (!(cin >> phone)) {
        cout << "Invalid input. Exiting.\n";
        return 0;
    }
    cin.ignore();   // flush leftover newline from cin>>
    cout << "Enter your name   : ";
    string custName;
    if (!getline(cin, custName)) {
        cout << "Exiting.\n";
        return 0;
    }

    Customer user(custName, phone, &orchestrator);

    Logger::getInstance()->log("SYSTEM", "System initialized for customer: " + user.getName() + " (ID: " + to_string(user.getCustId()) + ")");

    // ── Main Menu Loop ─────────────────────────────────────────────────────
    while (true) {
        cout << "\n========================================" << endl;
        cout << "               MAIN MENU"                 << endl;
        cout << "========================================\n" << endl;
        cout << "[1] Browse Movies\n"
             << "[2] Cancel Booking\n"
             << "[3] My Tickets\n"
             << "[4] Exit\n"
             << "\nEnter your choice: ";

        int ch = 0;
        if (!(cin >> ch)) {
            // EOF or input closed
            Logger::getInstance()->log("SYSTEM", "Input stream closed. Exiting gracefully.");
            Logger::getInstance()->flush();
            DBConnection::closeConnection();
            cout << "\nGoodbye!\n";
            return 0;
        }

        switch (ch) {
        case 1:
            user.browseAndBookMovies(&user);
            break;

        case 2: {
            cout << "Enter Booking ID to cancel: ";
            int bookingId = 0;
            if (cin >> bookingId) {
                int result = user.cancelMyBooking(bookingId);
                if      (result == -1) cout << "Booking already cancelled.\n";
                else if (result ==  0) cout << "Invalid Booking ID.\n";
                else                   cout << "Booking cancelled successfully.\n";
            }
            break;
        }

        case 3:
            user.printMyTicket();
            break;

        case 4:
            Logger::getInstance()->log("SYSTEM", "User exited normally.");
            Logger::getInstance()->flush();
            DBConnection::closeConnection();
            cout << "Goodbye!\n";
            return 0;

        default:
            cout << "Invalid choice. Try again.\n";
            break;
        }
    }
    return 0;
}
