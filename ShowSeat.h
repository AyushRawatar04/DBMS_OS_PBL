#ifndef SHOWSEAT_H
#define SHOWSEAT_H

#include <string>
class Seat;

class ShowSeat {
private:
    Seat *seat;
    std::string seatStatus;
public:
    ShowSeat(Seat* seat);
    int getId();
    std::string getSeatType();
    Seat* getSeat();
    void setStatus(std::string status);
    std::string getStatus();
};

#endif
