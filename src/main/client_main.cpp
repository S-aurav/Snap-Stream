#include "../network/network_receiver.h"
#include "../decoder/decoder_ffmpeg.h"
#include "../renderer/renderer_opengl.h"

#include <iostream>
#include <thread>
#include <vector>
#include <cstdint>
// #include <SDL.h>

int main() {
    std::cout << "Initializing client..." << std::endl;
    // const int listenPort = 8888;

    // // Initialize receiver
    // NetworkReceiver receiver(listenPort);

    // // Initialize decoder
    // FFmpegDecoder decoder;
    // if (!decoder.init()) {
    //     std::cerr << "Decoder init failed!" << std::endl;
    //     return -1;
    // }

    // // Initialize OpenGL renderer
    // RendererOpenGL renderer(1920, 1080); // Initial resolution (adjust dynamically later)
    // if (!renderer.init(argc, argv)) {
    //     std::cerr << "OpenGL renderer init failed!" << std::endl;
    //     return -1;
    // }

    // std::vector<uint8_t> encodedData;

    // while (!renderer.shouldClose()) {
    //     if (receiver.receivePacket(encodedData)) {
    //         AVFrame* frame = decoder.decodeFrame(encodedData);
    //         if (frame) {
    //             renderer.render(frame);
    //             decoder.releaseFrame(frame);  // Free frame when done
    //         }
    //     }

    //     renderer.pollEvents();  // Handle window input
    // }

    // receiver.cleanup();
    // decoder.cleanup();
    // renderer.cleanup();

    return 0;
}
