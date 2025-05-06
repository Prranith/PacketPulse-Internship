#define _CRT_SECURE_NO_WARNINGS
#include "Logger.h"
#include <iomanip>
#include <sstream>
#include <iostream>

Logger* Logger::instance = nullptr;
std::mutex Logger::logMutex;

bool Logger::openLogFile() {
    const std::string logFilePath = "C:\\Logs\\file_operations.log";
    CreateDirectoryA("C:\\Logs", NULL);
    logFile.open(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file at " << logFilePath << "! Error: " << GetLastError() << std::endl;
        Sleep(1000);
        logFile.open(logFilePath, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file at " << logFilePath << " after retry! Error: " << GetLastError() << std::endl;
            return false;
        }
    }
    std::cout << "Successfully opened log file at " << logFilePath << std::endl;
    return true;
}

Logger& Logger::getInstance() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (!instance) {
        instance = new Logger();
    }
    return *instance;
}

void Logger::logPacket(const std::string& packetInfo) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile << packetInfo << "\n\n"; // Ensure double newline for separation
        logFile.flush();
        std::cout << "Logged packet:\n" << packetInfo << "\n\n";
    }
    else {
        std::cerr << "Log file is not open! Attempting to reopen..." << std::endl;
        if (openLogFile()) {
            logPacket(packetInfo);
        }
        else {
            std::cerr << "Failed to log packet after retry!" << std::endl;
        }
    }
}

void Logger::logFileAction(const std::string& action, const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        std::ostringstream oss;
        oss << "[" << st.wYear << "-" << std::setw(2) << std::setfill('0') << st.wMonth << "-"
            << std::setw(2) << std::setfill('0') << st.wDay << " "
            << std::setw(2) << std::setfill('0') << st.wHour << ":"
            << std::setw(2) << std::setfill('0') << st.wMinute << ":"
            << std::setw(2) << std::setfill('0') << st.wSecond << "] "
            << "FILE_ACTION: " << action << ": " << filename;
        logFile << oss.str() << "\n\n";
        logFile.flush();
        std::cout << "Logged file action: " << oss.str() << "\n\n";
    }
    else {
        std::cerr << "Log file is not open! Attempting to reopen..." << std::endl;
        if (openLogFile()) {
            logFileAction(action, filename);
        }
        else {
            std::cerr << "Failed to log file action after retry!" << std::endl;
        }
    }
}

void Logger::logFlowStats(const std::string& flowInfo) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream flowFile("C:\\Logs\\flow_stats.log", std::ios::app);
    if (flowFile.is_open()) {
        flowFile << flowInfo << "\n\n";
        flowFile.close();
        std::cout << "Logged flow stats:\n" << flowInfo << "\n\n";
    }
    else {
        std::cerr << "Failed to open flow stats log file!" << std::endl;
    }
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile.close();
        std::cout << "Log file closed successfully" << std::endl;
    }
    delete instance;
    instance = nullptr;
}