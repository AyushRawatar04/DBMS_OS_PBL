#include "Screen.h"
#include <iostream>

using namespace std;

int Screen::seatNo = 1;

Screen::Screen(int screenNumber, int Silver, int Gold, int Platinum) {
    this->screenNumber = screenNumber;
    this->Silver = Silver;
    this->Gold = Gold;
    this->Platinum = Platinum;
    seatNo=1;
    setSeats();
}

void Screen::setSeats() {
    // Setting silver seats in the hall
    for(int i=0; i<Silver; i++){
        seats.push_back(Seat("Silver", seatNo));
        seatNo++;
    }

    // Setting gold seats in the hall
    for(int i=0; i<Gold; i++){
        seats.push_back(Seat("Gold", seatNo));
        seatNo++;
    }

    // Setting platinum seats in the hall
    for(int i=0; i<Platinum; i++){
        seats.push_back(Seat("Platinum", seatNo));
        seatNo++;
    }
}

vector<Seat>& Screen::getSeats() {
    return seats;
}

int Screen::getScreenNumber() {
    return screenNumber;
}

int Screen::getTotalSeats() {
    return (Silver + Gold + Platinum);
}

void Screen::displayHallInfo() {
    cout << " Screen Number=" << screenNumber << endl;
    cout << " total Silver Seats=" << Silver << endl;
    cout << " total Gold Seats=" << Gold << endl;
    cout << " total Platinum Seats=" << Platinum << endl;
}
