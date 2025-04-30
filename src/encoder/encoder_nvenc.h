// encoder_nvenc.h
#pragma once
#include <vector>
#include <cstdint>
#include <d3d11.h>

bool init_encoder(int width, int height);
bool encode_frame(ID3D11Texture2D* frame, std::vector<uint8_t>& outPacket);
void close_encoder();
