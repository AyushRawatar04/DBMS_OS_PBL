#include "Show.h"
#include "Movie.h"
#include "Screen.h"
#include "Seat.h"
#include "DBConnection.h"
#include "Logger.h"
#include <iostream>
#include <mysql/mysql.h>

using namespace std;

void Show::setShow(int startTime, Screen* screen, Movie* movie, int showId) {
    this->showId = showId;
    this->startTime = startTime;
    this->endTime = startTime + movie->getDuration();
    this->showScreen = screen;
    this->movie = movie;

    vector<Seat>& seats = screen->getSeats();

    for(auto &seat : seats){
        showSeats.push_back(ShowSeat(&seat));
    }

    // If showId is set, sync booked seats from show_seats table
    if (showId > 0) {
        MYSQL* conn = DBConnection::getInstance()->getConnection();
        char query[256];
        snprintf(query, sizeof(query),
            "SELECT seat_number, status FROM show_seats WHERE show_id = %d", showId);

        if (mysql_query(conn, query) == 0) {
            MYSQL_RES* res = mysql_store_result(conn);
            if (res) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(res))) {
                    int seatNum = stoi(row[0]);
                    string status = row[1];
                    if (status != "available" && status != "Available") {
                        for (auto& ss : showSeats) {
                            if (ss.getId() == seatNum) {
                                ss.setStatus("Booked");
                            }
                        }
                    }
                }
                mysql_free_result(res);
            }
        }
    }
}

int Show::getShowId() {
    return showId;
}

void Show::setShowId(int id) {
    this->showId = id;
}

vector<ShowSeat>& Show::getShowSeats() {
    return showSeats;
}

int Show::getScreenId() {
    return showScreen->getScreenNumber();
}

int Show::getEndTime() {
    return endTime;
}

int Show::getStartTime() {
    return startTime;
}

string Show::getMovieName() {
    return movie->getTitle();
}

void Show::getScreenInfo() {
    showScreen->displayHallInfo();
}

bool Show::checkAvailability(vector<int>& seatNumbers) {
    for(int seatId : seatNumbers){
        for(ShowSeat &showSeat : showSeats){
            if(showSeat.getId() == seatId){
                if(showSeat.getStatus() != "Available"){
                    cout << "---------Seat is already Booked , your booking has been cancelled------" << endl;
                    return false;
                }
            }
        }
    }
    return true;
}

bool Show::checkValid(vector<int>& seatNumbers) {
    int totalSeats = showScreen->getTotalSeats();
    for(auto val : seatNumbers){
        if(val > totalSeats || val<=0){
            cout << "-------Selected Seat is invalid, your booking has been cancelled-------" << endl;
            return false;
        }
    }
    return true;
}

bool Show::bookSeat(vector<int>& seatNumbers) {
    for(int seatId : seatNumbers){
        for(ShowSeat &showSeat : showSeats){
            if(showSeat.getId() == seatId){
                showSeat.setStatus("Booked");
            }
        }
    }    
    return true;
}

void Show::displaySection(string type) {
    cout << "             " << type << "\n\n";

    for (ShowSeat &showSeat : showSeats) {
        if (showSeat.getSeat()->getType() != type) {
            continue;
        }

        if (showSeat.getStatus() == "Available") {
            cout << "[" << showSeat.getSeat()->getSeatNumber() << "] ";
        }
        else {
            cout << "[XX] ";
        }
    }

    cout << "\n\n";
}

void Show::displaySeats() {
    cout << "\n";
    cout << "========================================" << endl;
    cout << "                 SCREEN" << endl;
    cout << "========================================\n" << endl;

    displaySection("Silver");
    displaySection("Gold");
    displaySection("Platinum");

    cout << "----------------------------------------" << endl;
    cout << " [01] Available       [XX] Booked" << endl;
    cout << "----------------------------------------\n" << endl;
}
