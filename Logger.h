#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <windows.h>

class Logger {
private:
    static Logger* instance;
    static std::mutex logMutex;
    std::ofstream logFile;

    Logger() {
        openLogFile();
    }

    bool openLogFile();

public:
    static Logger& getInstance();
    void logPacket(const std::string& packetInfo);
    void logFileAction(const std::string& action, const std::string& filename);
    void logFlowStats(const std::string& flowInfo);
    ~Logger();
};

#endif // LOGGER_H