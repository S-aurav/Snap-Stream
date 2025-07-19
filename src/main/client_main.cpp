#define SDL_MAIN_HANDLED

#include "../network/network_receiver.h"
#include "../decoder/decoder_ffmpeg.h"
#include "../renderer/renderer_opengl.h"

#include <iostream>
#include <thread>
#include <vector>
#include <cstdint>

extern "C" {
    #include <libavutil/log.h>  
}

int main() {
    av_log_set_level(AV_LOG_DEBUG);

    NetworkReceiver receiver(8888);
    FFmpegDecoder decoder;
    RendererOpenGL renderer(1920, 1080);

    if (!receiver.init() || !decoder.init() || !renderer.init()) {
        std::cerr << "Initialization failed" << std::endl;
        return -1;
    }

    std::vector<uint8_t> encodedData;
    int consecutiveFailures = 0;
    const int MAX_FAILURES = 10;

    while (!renderer.shouldClose()) {
        if (receiver.receivePacket(encodedData)) {
            AVFrame* frame = decoder.decodeFrame(encodedData);
            if (frame) {
                renderer.render(frame);
                decoder.releaseFrame(frame);
                consecutiveFailures = 0;
            } else {
                if (++consecutiveFailures >= MAX_FAILURES) {
                    std::cerr << "Critical: Too many consecutive decode failures" << std::endl;
                    break;
                }
            }
        }
        renderer.pollEvents();
    }

    receiver.cleanup();
    decoder.cleanup();
    renderer.cleanup();
    return 0;
}