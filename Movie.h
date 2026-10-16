#ifndef MOVIE_H
#define MOVIE_H

#include <string>

class Movie {
private:
    std::string title, language;
    int duration;

public:
    Movie();
    Movie(std::string title, std::string language, int duration);
    std::string getTitle();
    int getDuration();
    void displayDetails();
};

#endif
