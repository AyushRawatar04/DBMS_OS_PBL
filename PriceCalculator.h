#ifndef PRICECALCULATOR_H
#define PRICECALCULATOR_H

#include <vector>
#include "ShowSeat.h"

class PriceCalculator {
private:
    int totalAmount;
public:
    PriceCalculator();
    int calculate(std::vector<int>& seatsToBook, std::vector<ShowSeat>& showSeats);
};

#endif