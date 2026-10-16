#include "Payment.h"
#include <iostream>

using namespace std;

void UPI::pay(int amount) {
    cout << "\n----------------------------------------" << endl;
      cout << "          PAYMENT SUCCESSFUL" << endl;
      cout << "----------------------------------------" << endl;
      cout << "Method : UPI" << endl;
      cout << "Amount : Rs. " << amount << endl;
      cout << "----------------------------------------\n" << endl;
}

void CreditCard::pay(int amount) {
    cout << "\n----------------------------------------" << endl;
      cout << "          PAYMENT SUCCESSFUL" << endl;
      cout << "----------------------------------------" << endl;
      cout << "Method : Credit Card" << endl;
      cout << "Amount : Rs. " << amount << endl;
      cout << "----------------------------------------\n" << endl;
}

void CashPayment::pay(int amount) {
    cout << "\n----------------------------------------" << endl;
      cout << "          PAYMENT SUCCESSFUL" << endl;
      cout << "----------------------------------------" << endl;
      cout << "Method : Cash" << endl;
      cout << "Amount : Rs. " << amount << endl;
      cout << "----------------------------------------\n" << endl;
}
