// network_sender.cpp
#include "network_sender.h"
#include <winsock2.h>

static SOCKET sock;
static sockaddr_in clientAddr;

bool init_sender(const char* ip, int port) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(port);
    clientAddr.sin_addr.s_addr = inet_addr(ip);
    return sock != INVALID_SOCKET;
}

void send_data(const std::vector<uint8_t>& packet) {
    sendto(sock, reinterpret_cast<const char*>(packet.data()), packet.size(), 0,
           (sockaddr*)&clientAddr, sizeof(clientAddr));
}

void close_sender() {
    closesocket(sock);
    WSACleanup();
}
