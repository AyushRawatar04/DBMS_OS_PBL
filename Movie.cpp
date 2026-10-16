#include "Movie.h"
#include <iostream>

using namespace std;

Movie::Movie() {}

Movie::Movie(string title, string language, int duration) {
    this->title = title;
    this->language = language;
    this->duration = duration;
}

string Movie::getTitle() {
    return title;
}

int Movie::getDuration() {
    return duration;
}

void Movie::displayDetails() {
    cout << " Movie Name = " << title << endl;
    cout << " Language = " << language << endl;
    cout << " Duration = " << duration << "hrs" << endl;
}
