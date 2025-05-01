// network_sender.h
#pragma once
#include <winsock2.h>
#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include <ws2tcpip.h>

class NetworkSender {
public:
    NetworkSender(const std::string& ip, int port);
    bool init();
    bool sendPacket(const std::vector<uint8_t>& data);
    void cleanup();
private:
    std::string serverIp;
    int serverPort;
    SOCKET sock;
};
