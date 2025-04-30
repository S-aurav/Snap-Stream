// network_sender.h
#pragma once
#include <vector>
#include <cstdint>

bool init_sender(const char* ip, int port);
void send_data(const std::vector<uint8_t>& packet);
void close_sender();
