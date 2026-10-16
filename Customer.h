#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>

class BookingService;

class Customer {
private:
    std::string name;
    long long phoneNo;
    BookingService *bookingService;
    int custId;
    static int nextCustId;

public:
    Customer(std::string name, long long phoneNo, BookingService *bookingService);
    int cancelMyBooking(int bookingId);
    void printMyTicket();
    int getCustId();
    std::string getName();
    long long getPhoneNo();
    void browseAndBookMovies(Customer *customer);
};

#endif
