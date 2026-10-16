#ifndef CINEMA_H
#define CINEMA_H

#include <string>
#include <vector>
#include "Screen.h"

class Cinema {
private:
    std::string cinemaName;
    std::vector<Screen> screens;

public:
    Cinema(std::string cinemaName);
    void addScreens();
    Screen* getScreen(int screenId);
    int screenCount();
};

#endif
