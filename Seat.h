#ifndef SEAT_H
#define SEAT_H

#include <string>

class Seat {
private:
    std::string type;
    int number;
public:
    Seat(std::string type, int number);
    int getSeatNumber();
    std::string getType();
    void displaySeatInfo();
};

#endif
