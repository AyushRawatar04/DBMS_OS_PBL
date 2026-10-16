#include "BookingService.h"
#include "Cinema.h"
#include "Customer.h"
#include "Payment.h"
#include "PriceCalculator.h"
#include "TicketPrinter.h"
#include "DBConnection.h"
#include "Logger.h"
#include <iostream>
#include <mysql/mysql.h>

using namespace std;

// ─────────────────────────────────────────────
//  System Initialization
// ─────────────────────────────────────────────

void BookingService::initializeSystem(Cinema *PVR, vector<Show> shows) {
    this->PVR  = PVR;
    allShows   = shows;
    loadBookingsFromDB();
}

void BookingService::loadBookingsFromDB() {
    MYSQL* conn = DBConnection::getInstance()->getConnection();
    const char* q =
        "SELECT b.booking_id, b.cust_id, b.show_id, b.total_amount, b.booking_status "
        "FROM bookings b ORDER BY b.booking_id";

    if (mysql_query(conn, q)) {
        Logger::getInstance()->log("ERROR", string("loadBookingsFromDB query failed: ") + mysql_error(conn));
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        int bId     = stoi(row[0]);
        int cId     = stoi(row[1]);
        int sId     = stoi(row[2]);
        int amt     = stoi(row[3]);
        bool status = (stoi(row[4]) == 1);

        Show* showPtr = nullptr;
        for (auto& s : allShows) {
            if (s.getShowId() == sId) {
                showPtr = &s;
                break;
            }
        }
        if (!showPtr) continue;

        char seatQ[256];
        snprintf(seatQ, sizeof(seatQ),
            "SELECT ss.seat_number FROM booking_seats bs "
            "JOIN show_seats ss ON bs.show_seat_id = ss.show_seat_id "
            "WHERE bs.booking_id = %d", bId);

        vector<int> bookedSeats;
        if (mysql_query(conn, seatQ) == 0) {
            MYSQL_RES* seatRes = mysql_store_result(conn);
            if (seatRes) {
                MYSQL_ROW sRow;
                while ((sRow = mysql_fetch_row(seatRes))) {
                    bookedSeats.push_back(stoi(sRow[0]));
                }
                mysql_free_result(seatRes);
            }
        }

        if (status) {
            showPtr->bookSeat(bookedSeats);
        }

        Booking b(showPtr, status, bookedSeats, amt, cId);
        b.setBookingId(bId);
        systemBookings.push_back(b);
    }
    mysql_free_result(res);
    Logger::getInstance()->log("INFO", "Loaded " + to_string(systemBookings.size()) + " existing bookings from DB.");
}

// ─────────────────────────────────────────────
//  Ticket Printing
// ─────────────────────────────────────────────

void BookingService::printCustomerTicket(Customer *customer) {
    TicketPrinter printer;
    printer.printTicket(customer, systemBookings);
}

// ─────────────────────────────────────────────
//  Payment  (returns true + fills methodOut)
// ─────────────────────────────────────────────

bool BookingService::payForSeats(int amount, string& methodOut) {
    cout << "\n========================================" << endl;
    cout << "                PAYMENT"                   << endl;
    cout << "========================================\n" << endl;
    cout << "Select Payment Method:\n"
         << "[1] UPI\n[2] Credit Card\n[3] Cash\n"
         << "\nEnter choice: ";

    int paymentMethod = 0;
    cin  >> paymentMethod;
    Payment *payment = nullptr;

    switch (paymentMethod) {
        case 1:  payment = new UPI();         methodOut = "UPI";        break;
        case 2:  payment = new CreditCard();  methodOut = "CreditCard"; break;
        case 3:  payment = new CashPayment(); methodOut = "Cash";       break;
        default: return false;
    }
    payment->pay(amount);
    delete payment;
    return true;
}

// ─────────────────────────────────────────────
//  DB Helper — get show_id from shows table
// ─────────────────────────────────────────────

int BookingService::dbGetShowId(int startTime, int screenId, const string& movieName) {
    MYSQL* conn = DBConnection::getInstance()->getConnection();
    char query[512];
    snprintf(query, sizeof(query),
        "SELECT s.show_id FROM shows s "
        "JOIN movies m ON s.movie_id = m.movie_id "
        "JOIN screens sc ON s.screen_id = sc.screen_id "
        "WHERE s.start_time = %d AND sc.screen_number = %d AND m.title = '%s'",
        startTime, screenId, movieName.c_str());

    if (mysql_query(conn, query)) {
        Logger::getInstance()->log("ERROR", string("dbGetShowId query failed: ") + mysql_error(conn));
        return -1;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return -1;
    MYSQL_ROW row = mysql_fetch_row(res);
    int showId = row ? stoi(row[0]) : -1;
    mysql_free_result(res);
    return showId;
}

// ─────────────────────────────────────────────
//  DB Helper — INSERT into bookings table
// ─────────────────────────────────────────────

int BookingService::dbInsertBooking(int custId, int showId, int amount) {
    MYSQL* conn = DBConnection::getInstance()->getConnection();
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO bookings (cust_id, show_id, total_amount, booking_status) "
        "VALUES (%d, %d, %d, 1)",
        custId, showId, amount);

    if (mysql_query(conn, query)) {
        Logger::getInstance()->log("ERROR", string("dbInsertBooking failed: ") + mysql_error(conn));
        return -1;
    }
    return (int)mysql_insert_id(conn);
}

// ─────────────────────────────────────────────
//  DB Helper — INSERT into booking_seats
// ─────────────────────────────────────────────

void BookingService::dbInsertBookingSeats(int bookingId, int showId, const vector<int>& seatIds) {
    MYSQL* conn = DBConnection::getInstance()->getConnection();
    for (int seatNum : seatIds) {
        // Find the show_seat_id for this seat in this show
        char query[512];
        snprintf(query, sizeof(query),
            "SELECT show_seat_id FROM show_seats "
            "WHERE show_id = %d AND seat_number = %d LIMIT 1",
            showId, seatNum);

        if (mysql_query(conn, query)) {
            Logger::getInstance()->log("ERROR", string("dbInsertBookingSeats select failed: ") + mysql_error(conn));
            continue;
        }
        MYSQL_RES* res = mysql_store_result(conn);
        if (!res) continue;
        MYSQL_ROW row = mysql_fetch_row(res);
        if (!row) { mysql_free_result(res); continue; }
        int showSeatId = stoi(row[0]);
        mysql_free_result(res);

        char insQuery[256];
        snprintf(insQuery, sizeof(insQuery),
            "INSERT INTO booking_seats (booking_id, show_seat_id) VALUES (%d, %d)",
            bookingId, showSeatId);
        if (mysql_query(conn, insQuery)) {
            Logger::getInstance()->log("ERROR", string("booking_seats insert failed: ") + mysql_error(conn));
        }
    }
}

// ─────────────────────────────────────────────
//  DB Helper — INSERT into payments table
// ─────────────────────────────────────────────

void BookingService::dbInsertPayment(int bookingId, int amount, const string& method) {
    MYSQL* conn = DBConnection::getInstance()->getConnection();
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO payments (booking_id, amount, payment_method) "
        "VALUES (%d, %d, '%s')",
        bookingId, amount, method.c_str());
    if (mysql_query(conn, query)) {
        Logger::getInstance()->log("ERROR", string("dbInsertPayment failed: ") + mysql_error(conn));
    }
}

// ─────────────────────────────────────────────
//  DB Helper — UPDATE show_seats status
// ─────────────────────────────────────────────

void BookingService::dbUpdateShowSeats(int showId, const vector<int>& seatIds, const string& status) {
    MYSQL* conn = DBConnection::getInstance()->getConnection();
    for (int seatNum : seatIds) {
        char query[512];
        snprintf(query, sizeof(query),
            "UPDATE show_seats SET status = '%s' "
            "WHERE show_id = %d AND seat_number = %d",
            status.c_str(), showId, seatNum);
        if (mysql_query(conn, query)) {
            Logger::getInstance()->log("ERROR", string("dbUpdateShowSeats failed: ") + mysql_error(conn));
        }
    }
}

// ─────────────────────────────────────────────
//  DB Helper — Cancel booking in DB
// ─────────────────────────────────────────────

void BookingService::dbCancelBooking(int bookingId, int showId, const vector<int>& seatIds) {
    MYSQL* conn = DBConnection::getInstance()->getConnection();
    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE bookings SET booking_status = 0 WHERE booking_id = %d", bookingId);
    if (mysql_query(conn, query)) {
        Logger::getInstance()->log("ERROR", string("dbCancelBooking update failed: ") + mysql_error(conn));
        return;
    }
    dbUpdateShowSeats(showId, seatIds, "available");
}

// ─────────────────────────────────────────────
//  Core: Book Show Seats
//  OS Concept: std::mutex guards the critical section
//  (seat availability check + seat reservation + DB write)
//  to prevent race conditions if two users try to book
//  the same seat concurrently.
// ─────────────────────────────────────────────

void BookingService::bookShowSeats(Show *selectedShow, Customer *customer) {
    cout << "\n========================================" << endl;
    cout << "              BOOK SEATS"                  << endl;
    cout << "========================================\n" << endl;

    cout << "Enter the number of seats to book: ";
    int cntOfSeats = 0;
    cin  >> cntOfSeats;
    vector<int> seatsToBook(cntOfSeats);
    for (int i = 0; i < cntOfSeats; i++) {
        cout << "Enter seat number: ";
        cin  >> seatsToBook[i];
    }

    // ── OS Critical Section: Lock mutex before check+book ──────────────────
    lock_guard<mutex> lock(seatMutex);
    Logger::getInstance()->log("INFO", "Seat lock acquired for customer " + to_string(customer->getCustId()));

    if (selectedShow->checkValid(seatsToBook) &&
        selectedShow->checkAvailability(seatsToBook))
    {
        PriceCalculator price;
        int bookingPrice = price.calculate(seatsToBook, selectedShow->getShowSeats());
        cout << "\n========================================" << endl;
        cout << "          TOTAL AMOUNT : Rs. " << bookingPrice << endl;
        cout << "========================================\n" << endl;

        string payMethod;
        if (payForSeats(bookingPrice, payMethod)) {
            // 1. Update in-memory state
            selectedShow->bookSeat(seatsToBook);

            // 2. Persist to DB ─────────────────────────────
            int showId = selectedShow->getShowId();
            if (showId <= 0) {
                showId = dbGetShowId(
                    selectedShow->getStartTime(),
                    selectedShow->getScreenId(),
                    selectedShow->getMovieName()
                );
            }

            int dbBookingId = -1;
            if (showId != -1) {
                dbBookingId = dbInsertBooking(customer->getCustId(), showId, bookingPrice);
                if (dbBookingId != -1) {
                    dbInsertBookingSeats(dbBookingId, showId, seatsToBook);
                    dbInsertPayment(dbBookingId, bookingPrice, payMethod);
                    dbUpdateShowSeats(showId, seatsToBook, "booked");
                }
            }

            // 3. Create Booking object with DB booking ID
            Booking booking(selectedShow, true, seatsToBook, bookingPrice, customer->getCustId());
            if (dbBookingId != -1) {
                booking.setBookingId(dbBookingId);
            }
            systemBookings.push_back(booking);

            Logger::getInstance()->log("BOOKING",
                "Customer " + to_string(customer->getCustId()) +
                " booked seats via " + payMethod +
                " | Amount: Rs." + to_string(bookingPrice) +
                " | DB BookingID: " + to_string(dbBookingId));

        } else {
            Booking booking(selectedShow, false, seatsToBook, bookingPrice, customer->getCustId());
            systemBookings.push_back(booking);
            Logger::getInstance()->log("WARN", "Payment failed for customer " + to_string(customer->getCustId()));
        }
    }
    // ── mutex released automatically here (lock_guard destructor) ──────────
    Logger::getInstance()->log("INFO", "Seat lock released for customer " + to_string(customer->getCustId()));
}

// ─────────────────────────────────────────────
//  Show selection flow
// ─────────────────────────────────────────────

void BookingService::select_Show_For_The_Movie(int choice, string movie, Customer *customer) {
    int sNo = 1;
    Show *selectedShow = nullptr;
    for (int i = 0; i < (int)allShows.size(); i++) {
        if (allShows[i].getMovieName() == movie) {
            if (choice == sNo) {
                selectedShow = &allShows[i];
                selectedShow->displaySeats();
                break;
            }
            sNo++;
        }
    }
    if (selectedShow) {
        bookShowSeats(selectedShow, customer);
    }
}

void BookingService::find_All_Show_For_Selected_Movie(string &movie, Customer *customer) {
    cout << "\n========================================" << endl;
    cout << "              SELECT A SHOW"               << endl;
    cout << "========================================" << endl;
    cout << "Movie : " << movie << "\n" << endl;

    int sNo = 1;
    for (int i = 0; i < (int)allShows.size(); i++) {
        if (allShows[i].getMovieName() == movie) {
            cout << "[" << sNo << "] "
                 << "Screen : " << allShows[i].getScreenId()
                 << "    Time : " << allShows[i].getStartTime()
                 << " - "        << allShows[i].getEndTime() << endl;
            sNo++;
        }
    }

    cout << "\nEnter your choice: ";
    int choice;
    cin  >> choice;
    select_Show_For_The_Movie(choice, movie, customer);
}

// ─────────────────────────────────────────────
//  Display movies — fetched dynamically from DB
// ─────────────────────────────────────────────

void BookingService::displayMovies(Customer *customer) {
    MYSQL* conn = DBConnection::getInstance()->getConnection();

    cout << "\n========================================" << endl;
    cout << "             SELECT A MOVIE"               << endl;
    cout << "========================================" << endl;

    const char* query = "SELECT movie_id, title, language, duration_mins FROM movies ORDER BY movie_id";
    if (mysql_query(conn, query)) {
        Logger::getInstance()->log("ERROR", string("displayMovies query failed: ") + mysql_error(conn));
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return;

    vector<string> movieTitles;
    MYSQL_ROW row;
    int idx = 1;
    while ((row = mysql_fetch_row(res))) {
        cout << "[" << idx << "] " << row[1]
             << "  (" << row[2] << ", " << row[3] << " hrs)" << endl;
        movieTitles.push_back(row[1]);
        idx++;
    }
    mysql_free_result(res);

    cout << "\nEnter your choice: ";
    int movieChoice = 0;
    cin  >> movieChoice;

    if (movieChoice < 1 || movieChoice > (int)movieTitles.size()) {
        cout << "********--Wrong input--********" << endl;
        return;
    }

    string movie = movieTitles[movieChoice - 1];
    find_All_Show_For_Selected_Movie(movie, customer);
}

// selectMovie kept for compatibility but now unused (displayMovies handles it)
void BookingService::selectMovie(int movieChoice, Customer *customer) {
    (void)movieChoice; (void)customer;
}

// ─────────────────────────────────────────────
//  Cancel Booking
//  Updates in-memory state AND DB
// ─────────────────────────────────────────────

int BookingService::CancelBooking(int bookingId) {
    int flag = 0;
    for (Booking &booking : systemBookings) {
        if (booking.getBookingId() == bookingId) {
            flag = 1;
            if (booking.getBookingStatus() == false) {
                return -1; // already cancelled
            }

            // 1. Update in-memory ShowSeats
            vector<ShowSeat> &showSeats = booking.getShow()->getShowSeats();
            for (int seatId : booking.getSeatIds()) {
                for (ShowSeat &ss : showSeats) {
                    if (ss.getId() == seatId) {
                        ss.setStatus("Available");
                    }
                }
            }
            booking.setBookingStatus(false);

            // 2. Get show_id and update DB
            int showId = booking.getShow()->getShowId();
            if (showId <= 0) {
                showId = dbGetShowId(
                    booking.getShow()->getStartTime(),
                    booking.getShow()->getScreenId(),
                    booking.getShow()->getMovieName()
                );
            }
            if (showId != -1) {
                dbCancelBooking(bookingId, showId, booking.getSeatIds());
            }

            Logger::getInstance()->log("CANCEL",
                "Booking " + to_string(bookingId) +
                " cancelled by customer " + to_string(booking.getCustomerId()));
        }
    }
    return (flag == 1) ? 1 : 0;
}