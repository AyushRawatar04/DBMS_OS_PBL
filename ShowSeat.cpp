#include "ShowSeat.h"
#include "Seat.h"

using namespace std;

ShowSeat::ShowSeat(Seat* seat) {
    this->seat = seat;
    seatStatus = "Available";
}

int ShowSeat::getId() {
    return seat->getSeatNumber();
}

string ShowSeat::getSeatType() {
    return seat->getType();
}

Seat* ShowSeat::getSeat() {
    return seat;
}

void ShowSeat::setStatus(string status) {
    seatStatus = status;
}

string ShowSeat::getStatus() {
    return seatStatus;
}
