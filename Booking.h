#ifndef BOOKING_H
#define BOOKING_H

#include <vector>

class Show; 

class Booking {
private:
    int bookingId;
    int totalAmount;
    int customerId;
    bool bookingStatus;
    Show* show;
    std::vector<int> seatIds;
    static int nextBookingId;
public:
    Booking(Show* show, bool bookingStatus, std::vector<int>& seatIds, int amount, int custId);
    std::vector<int>& getSeatIds();
    void setBookingStatus(bool status);
    int getBookingId();
    void setBookingId(int id);
    int getTotalAmount();
    bool getBookingStatus();
    Show* getShow();
    int getCustomerId();
};

#endif
