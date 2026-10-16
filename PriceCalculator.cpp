#include "PriceCalculator.h"

using namespace std;

PriceCalculator::PriceCalculator() {
    totalAmount = 0;
}

int PriceCalculator::calculate(vector<int>& seatsToBook, vector<ShowSeat>& showSeats) {
    for(int seatNo : seatsToBook){
        for(auto &showSeat : showSeats){
            if(seatNo == showSeat.getId()){
                if(showSeat.getSeatType() == "Platinum"){
                    totalAmount += 400;
                }
                else if(showSeat.getSeatType() == "Gold"){
                    totalAmount += 300;
                }
                else{
                    totalAmount += 200;
                }
            }
        }
    }
    return totalAmount;
}
