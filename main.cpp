#define _CRT_SECURE_NO_WARNINGS
#include "PacketSniffer.h"
#include "FileMonitor.h"
#include <iostream>
#include <string>
#include <thread>

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return -1;
    }

    PacketSniffer sniffer;
    sniffer.listInterfaces();
    int selectedIndex;

    std::cout << "Select interface index: " << std::flush;
    std::cin >> selectedIndex;
    std::cin.ignore();

    std::wstring folderPath;
    std::wcout << L"Enter full path of shared folder (e.g., C:\\SharedFolder): " << std::flush;
    std::getline(std::wcin, folderPath);

    std::thread sniffThread(&PacketSniffer::startSniffing, &sniffer, selectedIndex);
    FileMonitor fileMon(folderPath);
    std::thread monitorThread(&FileMonitor::startMonitoring, &fileMon);

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    // Signal threads to stop (simplified, actual termination may need pcap_breakloop)
    sniffThread.join();
    monitorThread.join();

    WSACleanup();
    return 0;
}