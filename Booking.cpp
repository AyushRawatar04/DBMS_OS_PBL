#include "Booking.h"
#include "Show.h"

using namespace std;

int Booking::nextBookingId = 101;

Booking::Booking(Show* show, bool bookingStatus, vector<int>& seatIds, int amount, int custId) {
    this->seatIds = seatIds;
    this->show = show;
    this->bookingStatus = bookingStatus;
    this->bookingId = nextBookingId;
    nextBookingId++;
    this->customerId = custId;
    this->totalAmount = amount;
}

void Booking::setBookingStatus(bool status){
    this->bookingStatus=status;
}
vector<int>& Booking::getSeatIds() {
    return seatIds;
}

int Booking::getBookingId() {
    return bookingId;
}

void Booking::setBookingId(int id) {
    this->bookingId = id;
}

int Booking::getTotalAmount() {
    return totalAmount;
}

bool Booking::getBookingStatus() {
    return bookingStatus;
}

Show* Booking::getShow() {
    return show;
}

int Booking::getCustomerId() {
    return customerId;
}
