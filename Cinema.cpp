#include "Cinema.h"
#include "DBConnection.h"
#include "Logger.h"
#include <iostream>
#include <mysql/mysql.h>

using namespace std;

Cinema::Cinema(string cinemaName) {
    this->cinemaName = cinemaName;
    addScreens();
}

// Load screens (and their seat counts) from the DB instead of hardcoding
void Cinema::addScreens() {
    MYSQL* conn = DBConnection::getInstance()->getConnection();

    const char* query =
        "SELECT screen_id, screen_number, silver_count, gold_count, platinum_count "
        "FROM screens ORDER BY screen_number";

    if (mysql_query(conn, query)) {
        Logger::getInstance()->log("ERROR", string("addScreens query failed: ") + mysql_error(conn));
        // Fallback to hardcoded defaults
        screens.push_back(Screen(1, 5, 5, 5));
        screens.push_back(Screen(2, 6, 6, 6));
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        Logger::getInstance()->log("ERROR", "addScreens: mysql_store_result failed");
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        int screenNum   = stoi(row[1]);
        int silverCnt   = stoi(row[2]);
        int goldCnt     = stoi(row[3]);
        int platinumCnt = stoi(row[4]);
        screens.push_back(Screen(screenNum, silverCnt, goldCnt, platinumCnt));
        Logger::getInstance()->log("INFO",
            "Loaded Screen " + to_string(screenNum) +
            " [S:" + to_string(silverCnt) +
            " G:" + to_string(goldCnt) +
            " P:" + to_string(platinumCnt) + "]");
    }
    mysql_free_result(res);

    if (screens.empty()) {
        Logger::getInstance()->log("WARN", "No screens found in DB. Using defaults.");
        screens.push_back(Screen(1, 5, 5, 5));
        screens.push_back(Screen(2, 6, 6, 6));
    }
}

Screen* Cinema::getScreen(int screenId) {
    for (auto &screen : screens) {
        if (screen.getScreenNumber() == screenId) {
            return &screen;
        }
    }
    return nullptr;
}

int Cinema::screenCount() {
    return screens.size();
}
