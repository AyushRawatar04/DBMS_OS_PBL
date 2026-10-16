#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <mysql/mysql.h>
#include <string>
#include <stdexcept>

// Singleton class that manages the MySQL database connection.
// OS Concept: Single resource handle shared across the process lifetime.
class DBConnection {
private:
    MYSQL* conn;
    static DBConnection* instance;

    DBConnection();
    ~DBConnection();
    DBConnection(const DBConnection&) = delete;
    DBConnection& operator=(const DBConnection&) = delete;

public:
    static DBConnection* getInstance();
    MYSQL* getConnection();
    static void closeConnection();
};

#endif
