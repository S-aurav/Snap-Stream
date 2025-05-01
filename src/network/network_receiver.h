#pragma once
#include <winsock2.h>
#include <vector>
#include <cstdint>

class NetworkReceiver {
public:
    NetworkReceiver(int port);
    bool init();
    void cleanup();
    bool receivePacket(std::vector<uint8_t>& outData);

private:
    int listenPort;
    SOCKET listenSocket;
    SOCKET clientSocket;
};
