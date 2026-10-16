#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

// OS Concept: File I/O for persistent event logging.
// OS Concept: std::mutex protects concurrent writes to the log file.
class Logger {
private:
    std::ofstream logFile;
    std::mutex logMutex;       // OS: mutual exclusion for thread-safe writes
    static Logger* instance;

    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

public:
    static Logger* getInstance();
    void log(const std::string& level, const std::string& message);
    void flush();
    ~Logger();
};

#endif
