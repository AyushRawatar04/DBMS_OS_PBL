#ifndef BOOKINGSERVICE_H
#define BOOKINGSERVICE_H

#include <vector>
#include <string>
#include <mutex>       // OS Concept: mutual exclusion for seat booking critical section
#include "Show.h"
#include "Booking.h"

class Cinema;
class Customer;

class BookingService {
private:
    Cinema *PVR;
    std::vector<Show> allShows;
    std::vector<Booking> systemBookings;

    // OS Concept: mutex prevents two threads from booking the same seat simultaneously
    std::mutex seatMutex;

    // DB helpers (internal)
    int  dbInsertBooking(int custId, int showId, int amount);
    void dbInsertBookingSeats(int bookingId, int showId, const std::vector<int>& seatIds);
    void dbInsertPayment(int bookingId, int amount, const std::string& method);
    void dbUpdateShowSeats(int showId, const std::vector<int>& seatIds, const std::string& status);
    void dbCancelBooking(int bookingId, int showId, const std::vector<int>& seatIds);
    int  dbGetShowId(int startTime, int screenId, const std::string& movieName);

public:
    void initializeSystem(Cinema *PVR, std::vector<Show> shows);
    void loadBookingsFromDB();
    void printCustomerTicket(Customer *customer);
    bool payForSeats(int amount, std::string& methodOut);
    void bookShowSeats(Show* selectedShow, Customer *customer);
    void select_Show_For_The_Movie(int choice, std::string movie, Customer *customer);
    void find_All_Show_For_Selected_Movie(std::string &movie, Customer *customer);
    void selectMovie(int movieChoice, Customer *customer);
    void displayMovies(Customer *customer);
    int  CancelBooking(int bookingId);
};

#endif
