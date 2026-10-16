#include "DBConnection.h"
#include <iostream>

using namespace std;

DBConnection* DBConnection::instance = nullptr;

DBConnection::DBConnection() {
    conn = mysql_init(nullptr);
    if (!conn) {
        throw runtime_error("[DB] MySQL init failed.");
    }
    // Connect: host=localhost, user=root, password="" (just Enter), db=cinema_booking
    if (!mysql_real_connect(conn, "localhost", "root", "", "cinema_booking", 3306, nullptr, 0)) {
        string err = mysql_error(conn);
        mysql_close(conn);
        throw runtime_error("[DB] Connection failed: " + err);
    }
    cout << "[DB] Connected to cinema_booking successfully.\n";
}

DBConnection::~DBConnection() {
    if (conn) {
        mysql_close(conn);
        conn = nullptr;
        cout << "[DB] Connection closed.\n";
    }
}

DBConnection* DBConnection::getInstance() {
    if (!instance) {
        instance = new DBConnection();
    }
    return instance;
}

MYSQL* DBConnection::getConnection() {
    return conn;
}

void DBConnection::closeConnection() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}
