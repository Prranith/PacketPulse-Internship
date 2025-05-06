#ifndef FILE_MONITOR_H
#define FILE_MONITOR_H

#include <windows.h>
#include <string>

class FileMonitor {
public:
    FileMonitor(const std::wstring& folder);
    ~FileMonitor();
    void startMonitoring();

private:
    static DWORD WINAPI MonitorThread(LPVOID lpParam);
    void processChanges(DWORD dwBytes, char* buffer);

    HANDLE hDirectory;
    HANDLE hThread;
    std::wstring folderPath;
};

#endif // FILE_MONITOR_H










