#ifndef SCREEN_H
#define SCREEN_H

#include <vector>
#include "Seat.h"

class Screen {
private:
    int screenNumber;
    int Silver, Gold, Platinum;
    static int seatNo;
    std::vector<Seat> seats;

public:
    Screen(int screenNumber, int Silver, int Gold, int Platinum);
    void setSeats();
    std::vector<Seat>& getSeats();
    int getScreenNumber();
    int getTotalSeats();
    void displayHallInfo();
};

#endif
