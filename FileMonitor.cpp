#define _CRT_SECURE_NO_WARNINGS
#include "FileMonitor.h"
#include "Logger.h"
#include "utils.h"
#include <iostream>

FileMonitor::FileMonitor(const std::wstring& folder) : folderPath(folder), hDirectory(INVALID_HANDLE_VALUE), hThread(NULL) {}

FileMonitor::~FileMonitor() {
    if (hThread) {
        CloseHandle(hThread);
    }
    if (hDirectory != INVALID_HANDLE_VALUE) {
        CloseHandle(hDirectory);
    }
}

void FileMonitor::startMonitoring() {
    hDirectory = CreateFileW(
        folderPath.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);

    if (hDirectory == INVALID_HANDLE_VALUE) {
        std::wcout << L"Failed to open directory " << folderPath << L": " << GetLastError() << std::endl;
        return;
    }

    hThread = CreateThread(NULL, 0, MonitorThread, this, 0, NULL);
    if (!hThread) {
        std::wcout << L"Failed to create monitoring thread for " << folderPath << L": " << GetLastError() << std::endl;
        CloseHandle(hDirectory);
        hDirectory = INVALID_HANDLE_VALUE;
    }
}

DWORD WINAPI FileMonitor::MonitorThread(LPVOID lpParam) {
    FileMonitor* monitor = static_cast<FileMonitor*>(lpParam);
    HANDLE hDirectory = monitor->hDirectory;
    char* buffer = new char[1024 * 16];
    DWORD bytesReturned;

    OVERLAPPED overlapped = { 0 };
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!overlapped.hEvent) {
        std::wcout << L"CreateEvent failed: " << GetLastError() << std::endl;
        delete[] buffer;
        return 1;
    }

    std::wcout << L"Started monitoring directory: " << monitor->folderPath << std::endl;

    while (true) {
        if (!ReadDirectoryChangesW(
            hDirectory,
            buffer,
            1024 * 16,
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY,
            &bytesReturned,
            &overlapped,
            NULL)) {
            std::wcout << L"ReadDirectoryChangesW failed for " << monitor->folderPath << L": " << GetLastError() << std::endl;
            break;
        }

        WaitForSingleObject(overlapped.hEvent, INFINITE);
        if (bytesReturned > 0) {
            monitor->processChanges(bytesReturned, buffer);
        }
        if (!ResetEvent(overlapped.hEvent)) {
            std::wcout << L"ResetEvent failed: " << GetLastError() << std::endl;
        }
    }

    if (overlapped.hEvent) {
        CloseHandle(overlapped.hEvent);
    }
    delete[] buffer;
    return 0;
}

void FileMonitor::processChanges(DWORD dwBytes, char* buffer) {
    if (dwBytes == 0) {
        std::wcout << L"No change data received." << std::endl;
        return;
    }

    FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)buffer;
    do {
        std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
        std::wstring action;

        switch (pNotify->Action) {
        case FILE_ACTION_ADDED:
            action = L"Create Request";
            Logger::getInstance().logFileAction("Create Request", std::string(fileName.begin(), fileName.end()));
            break;
        case FILE_ACTION_REMOVED:
            action = L"Delete Request";
            Logger::getInstance().logFileAction("Delete Request", std::string(fileName.begin(), fileName.end()));
            break;
        case FILE_ACTION_MODIFIED:
            action = L"Update Request";
            Logger::getInstance().logFileAction("Update Request", std::string(fileName.begin(), fileName.end()));
            break;
        case FILE_ACTION_RENAMED_OLD_NAME:
            action = L"Rename Request (Old Name)";
            Logger::getInstance().logFileAction("Rename Request (Old Name)", std::string(fileName.begin(), fileName.end()));
            break;
        case FILE_ACTION_RENAMED_NEW_NAME:
            action = L"Rename Request (New Name)";
            Logger::getInstance().logFileAction("Rename Request (New Name)", std::string(fileName.begin(), fileName.end()));
            break;
        default:
            action = L"Unknown Request";
            Logger::getInstance().logFileAction("Unknown Request", std::string(fileName.begin(), fileName.end()));
        }

        std::wcout << L"[File Action] " << currentTimestampW() << L" | " << action << L": " << fileName << std::endl;

        if (pNotify->NextEntryOffset == 0) break;
        pNotify = (FILE_NOTIFY_INFORMATION*)((BYTE*)pNotify + pNotify->NextEntryOffset);
    } while (true);
}