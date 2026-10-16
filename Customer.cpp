#include "Customer.h"
#include "BookingService.h"
#include "DBConnection.h"
#include "Logger.h"
#include <iostream>
#include <mysql/mysql.h>

using namespace std;

int Customer::nextCustId = 1;

Customer::Customer(string name, long long phoneNo, BookingService *bookingService) {
    this->phoneNo        = phoneNo;
    this->bookingService = bookingService;

    MYSQL* conn = DBConnection::getInstance()->getConnection();

    // Check if customer already exists by phone
    char checkQuery[256];
    snprintf(checkQuery, sizeof(checkQuery),
        "SELECT cust_id, name FROM customers WHERE phone_no = %lld LIMIT 1", phoneNo);

    bool found = false;
    if (mysql_query(conn, checkQuery) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row) {
                this->custId = stoi(row[0]);
                this->name   = row[1];
                found = true;
                Logger::getInstance()->log("INFO",
                    "Existing customer found: " + this->name +
                    " | Phone: " + to_string(phoneNo) +
                    " | DB CustID: " + to_string(this->custId));
                cout << "\n>>> Welcome back, " << this->name << "! (Customer ID: " << this->custId << ") <<<\n";
            }
            mysql_free_result(res);
        }
    }

    if (!found) {
        this->name = name;
        char query[512];
        snprintf(query, sizeof(query),
            "INSERT INTO customers (name, phone_no) VALUES ('%s', %lld)",
            name.c_str(), phoneNo);

        if (mysql_query(conn, query)) {
            Logger::getInstance()->log("ERROR", string("Customer insert failed: ") + mysql_error(conn));
            this->custId = nextCustId++;
        } else {
            this->custId = (int)mysql_insert_id(conn);
            Logger::getInstance()->log("INFO",
                "New customer registered: " + name +
                " | Phone: " + to_string(phoneNo) +
                " | DB CustID: " + to_string(this->custId));
            cout << "\n>>> Welcome, " << this->name << "! (Customer ID: " << this->custId << ") <<<\n";
        }
    }
}

int Customer::cancelMyBooking(int bookingId) {
    return bookingService->CancelBooking(bookingId);
}

void Customer::printMyTicket() {
    bookingService->printCustomerTicket(this);
}

int Customer::getCustId() {
    return custId;
}

string Customer::getName() {
    return name;
}

long long Customer::getPhoneNo() {
    return phoneNo;
}

void Customer::browseAndBookMovies(Customer *customer) {
    bookingService->displayMovies(customer);
}
