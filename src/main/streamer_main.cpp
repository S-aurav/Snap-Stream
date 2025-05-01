extern "C" {
    #include <libavutil/log.h>  // Add this include
}

#include "../capture/capture_dxgi.h"
#include "../encoder/encoder_nvenc.h"
#include "../network/network_sender.h"
#include <iostream>
#include <fstream>



int main() {
    av_log_set_level(AV_LOG_DEBUG);

    // Initialize the network sender
    NetworkSender sender("127.0.0.1", 8888);
    if (!sender.init()) {
        std::cerr << "Failed to initialize network sender" << std::endl;
        return -1;
    }

    std::cout << "Initializing streamer..." <<std::endl;
    ScreenCapturer capturer;
    if (!capturer.Initialize()) return -1;

    if (!init_encoder(1920, 1080)) return -1;
    // if (!init_sender("127.0.0.1", 8888)) return -1;

    int frameCount = 0;
    while (true) {
        ID3D11Texture2D* frame = nullptr;
        if (capturer.CaptureFrame(&frame)) {
            std::vector<uint8_t> packet;
            if (encode_frame(frame, packet)) {
                // Verify packet structure
                if (packet.size() > 4) {
                    uint8_t nal_type = packet[4] & 0x1F;
                    std::cout << "Encoded packet: " << packet.size() 
                            << " bytes, NAL type: " << (int)nal_type 
                            << (nal_type == 5 ? " (IDR)" : "") << std::endl;
                }
                
                sender.sendPacket(packet);
                frameCount++;
                
                // // Save every 10th frame for debugging
                // if (frameCount % 10 == 0) {
                //     std::ofstream debug("frame_" + std::to_string(frameCount) + ".h264", 
                //                     std::ios::binary);
                //     debug.write(reinterpret_cast<const char*>(packet.data()), packet.size());
                // }
            }
            frame->Release();
        }
    }
    
   sender.cleanup();
    close_encoder();
    return 0;
}
