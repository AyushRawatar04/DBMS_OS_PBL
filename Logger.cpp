#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

Logger* Logger::instance = nullptr;

Logger::Logger() {
    // OS Concept: Open file in append mode — data persists across runs
    logFile.open("booking.log", ios::app);
    if (!logFile.is_open()) {
        cerr << "[Logger] WARNING: Could not open booking.log\n";
    } else {
        cout << "[Logger] Logging to booking.log\n";
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.flush();
        logFile.close();
    }
}

Logger* Logger::getInstance() {
    if (!instance) {
        instance = new Logger();
    }
    return instance;
}

void Logger::log(const string& level, const string& message) {
    // OS Concept: std::lock_guard acquires mutex — critical section for file write
    lock_guard<mutex> guard(logMutex);

    // Get current timestamp using OS clock
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    struct tm localTime;
    localtime_r(&t, &localTime);

    ostringstream ts;
    ts << put_time(&localTime, "%Y-%m-%d %H:%M:%S");

    string entry = "[" + ts.str() + "] [" + level + "] " + message;

    // Write to file (OS file I/O)
    if (logFile.is_open()) {
        logFile << entry << "\n";
        logFile.flush(); // Ensure OS flushes buffer to disk
    }
    // Also echo to console for visibility
    cout << entry << "\n";
}

void Logger::flush() {
    lock_guard<mutex> guard(logMutex);
    if (logFile.is_open()) {
        logFile.flush();
    }
}
