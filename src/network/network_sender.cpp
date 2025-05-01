// network_sender.cpp
#include "network_sender.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <cstdint>
#include <string>
#include <cstring> // For memcpy
#pragma comment(lib, "ws2_32.lib")

NetworkSender::NetworkSender(const std::string& ip, int port)
    : serverIp(ip), serverPort(port), sock(INVALID_SOCKET) {}

bool NetworkSender::init() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed\n";
        return false;
    }

    return true;
}

bool NetworkSender::sendPacket(const std::vector<uint8_t>& data) {
    int32_t size = static_cast<int32_t>(data.size());
    if (send(sock, reinterpret_cast<const char*>(&size), sizeof(size), 0) != sizeof(size)) {
        std::cerr << "Failed to send packet size\n";
        return false;
    }
    
    size_t sent = 0;
    while (sent < data.size()) {
        int chunk = send(sock, reinterpret_cast<const char*>(data.data()) + sent, data.size() - sent, 0);
        if (chunk <= 0) {
            std::cerr << "Failed to send packet data\n";
            return false;
        }
        sent += chunk;
    }
    return true;
}

void NetworkSender::cleanup() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        WSACleanup();
    }
}
