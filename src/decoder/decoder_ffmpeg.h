#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
}

#include <functional>

class FFmpegDecoder {
public:
    FFmpegDecoder();
    ~FFmpegDecoder();
    bool init();
    bool decode(const uint8_t* data, size_t size, std::function<void(AVFrame*)> onFrame);
    void cleanup();
    AVFrame* decodeFrame(const std::vector<uint8_t>& data);
    void releaseFrame(AVFrame* frame);

private:
    AVCodecContext* codecCtx;
    AVCodecParserContext* parser;
    const AVCodec* codec;
    AVFrame* frame;
    AVPacket* pkt;
};
