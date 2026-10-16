#include "Seat.h"
#include <iostream>

using namespace std;

Seat::Seat(string type, int number) {
    this->type = type;
    this->number = number;
}

int Seat::getSeatNumber() {
    return number;
}

string Seat::getType() {
    return type;
}

void Seat::displaySeatInfo() {
    cout << " Seat type = " << type << endl;
    cout << " Seat number = " << number << endl;
}
