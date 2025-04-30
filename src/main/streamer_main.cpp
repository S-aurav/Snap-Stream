#include "../capture/capture_dxgi.h"
#include "../encoder/encoder_nvenc.h"
#include "../network/network_sender.h"
#include <iostream>

int main() {
    std::cout << "Initializing streamer..." << std::endl;
    ScreenCapturer capturer;
    if (!capturer.Initialize()) return -1;

    if (!init_encoder(1920, 1080)) return -1;
    if (!init_sender("127.0.0.1", 8888)) return -1;

    while (true) {
        ID3D11Texture2D* frame = nullptr;
        if (capturer.CaptureFrame(&frame)) {
            std::vector<uint8_t> packet;
            if (encode_frame(frame, packet)) {
                send_data(packet);
            }
            frame->Release();
        }
    }

    close_sender();
    close_encoder();
    return 0;
}
