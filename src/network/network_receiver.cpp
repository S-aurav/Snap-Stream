// network_receiver.cpp
#include "network_receiver.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

NetworkReceiver::NetworkReceiver(int port)
    : listenPort(port), listenSocket(INVALID_SOCKET), clientSocket(INVALID_SOCKET) {}

bool NetworkReceiver::init() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    sockaddr_in recvAddr{};
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(listenPort);
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, (sockaddr*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        return false;
    }

    if (listen(listenSocket, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        return false;
    }

    std::cout << "Waiting for incoming connection on port " << listenPort << "...\n";
    clientSocket = accept(listenSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed\n";
        return false;
    }

    std::cout << "Client connected.\n";
    return true;
}

bool NetworkReceiver::receivePacket(std::vector<uint8_t>& outData) {
    int32_t packetSize = 0;
    int received = recv(clientSocket, reinterpret_cast<char*>(&packetSize), sizeof(packetSize), MSG_WAITALL);
    if (received != sizeof(packetSize)) {
        std::cerr << "Failed to receive packet size\n";
        return false;
    }

    outData.resize(packetSize);
    size_t totalReceived = 0;
    while (totalReceived < packetSize) {
        int chunk = recv(clientSocket, reinterpret_cast<char*>(outData.data()) + totalReceived, packetSize - totalReceived, 0);
        if (chunk <= 0) {
            std::cerr << "Failed to receive packet data\n";
            return false;
        }
        totalReceived += chunk;
    }
    return true;
}

void NetworkReceiver::cleanup() {
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
    }
    if (listenSocket != INVALID_SOCKET) {
        closesocket(listenSocket);
    }
    WSACleanup();
}
