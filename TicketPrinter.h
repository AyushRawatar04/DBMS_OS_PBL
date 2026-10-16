#ifndef TICKETPRINTER_H
#define TICKETPRINTER_H

#include <vector>
#include "Booking.h"

class Customer;

class TicketPrinter {
public:
    void printTicket(Customer *customer, std::vector<Booking>& systemBookings);
};

#endif
