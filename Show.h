#ifndef SHOW_H
#define SHOW_H

#include <vector>
#include <string>
#include "ShowSeat.h" 

class Screen;
class Movie;
class Seat;

class Show {
private:
    int showId;
    int startTime;
    int endTime;
    Screen* showScreen;
    Movie* movie;
    std::vector<ShowSeat> showSeats;

public:
    void setShow(int startTime, Screen* screen, Movie* movie, int showId = 0);
    int getShowId();
    void setShowId(int id);
    std::vector<ShowSeat>& getShowSeats();
    int getScreenId();
    int getEndTime();
    int getStartTime();
    std::string getMovieName();
    void getScreenInfo();
    bool checkAvailability(std::vector<int>& seatNumbers);
    bool checkValid(std::vector<int>& seatNumbers);
    bool bookSeat(std::vector<int>& seatNumbers);
    void displaySection(std::string type);
    void displaySeats();
};

#endif
