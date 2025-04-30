#include "network_receiver.h"
#include <iostream>
#include <cstdint>
#include <vector>
#include <WS2tcpip.h>

NetworkReceiver::NetworkReceiver(int port) : listenPort(port), listenSocket(INVALID_SOCKET) {}

bool NetworkReceiver::init() {
#ifdef _WIN32
    WSADATA wsaData;
    int wsaErr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaErr != 0) {
        std::cerr << "WSAStartup failed: " << wsaErr << std::endl;
        return false;
    }
#endif

    listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        return false;
    }

    sockaddr_in recvAddr {};
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(listenPort);
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, (sockaddr*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
        std::cerr << "Socket bind failed." << std::endl;
        cleanup();
        return false;
    }

    std::cout << "Listening on port " << listenPort << std::endl;
    return true;
}

bool NetworkReceiver::receivePacket(std::vector<uint8_t>& outData) {
    char buffer[65536];
    sockaddr_in senderAddr {};
    socklen_t senderAddrSize = sizeof(senderAddr);

    int recvLen = recvfrom(listenSocket, buffer, sizeof(buffer), 0,
                           (sockaddr*)&senderAddr, &senderAddrSize);
    if (recvLen == SOCKET_ERROR || recvLen <= 0) {
        return false;
    }

    outData.assign(buffer, buffer + recvLen);
    return true;
}

void NetworkReceiver::cleanup() {
    if (listenSocket != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(listenSocket);
        WSACleanup();
#else
        close(listenSocket);
#endif
        listenSocket = INVALID_SOCKET;
    }
}
