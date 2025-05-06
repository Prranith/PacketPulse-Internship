#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <windows.h>

inline std::string currentTimestamp(double timestamp) {
    time_t sec = static_cast<time_t>(timestamp);
    double frac = timestamp - sec;
    int usec = static_cast<int>(frac * 1000000);
    std::tm local;
    localtime_s(&local, &sec);
    std::ostringstream oss;
    oss << std::put_time(&local, "%Y-%m-%d %H:%M:%S") << "."
        << std::setw(6) << std::setfill('0') << usec;
    return oss.str();
}

inline std::wstring currentTimestampW() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    std::wostringstream oss;
    oss << L"[" << std::setw(2) << std::setfill(L'0') << st.wHour << L":"
        << std::setw(2) << std::setfill(L'0') << st.wMinute << L":"
        << std::setw(2) << std::setfill(L'0') << st.wSecond << L"]";
    return oss.str();
}