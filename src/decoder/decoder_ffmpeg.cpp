// For FFmpeg
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}

// For std::function
#include <functional>

// For WinSock
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")  // Link with ws2_32.lib
    #include "decoder_ffmpeg.h"
    
    FFmpegDecoder::FFmpegDecoder() : codecCtx(nullptr), parser(nullptr), codec(nullptr), frame(nullptr), pkt(nullptr) {}

    FFmpegDecoder::~FFmpegDecoder(){
        cleanup();
    }

    bool FFmpegDecoder::init() {
        codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        if (!codec) return false;
    
        parser = av_parser_init(codec->id);
        if (!parser) return false;
    
        codecCtx = avcodec_alloc_context3(codec);
        if (!codecCtx) return false;
    
        if (avcodec_open2(codecCtx, codec, nullptr) < 0) return false;
    
        frame = av_frame_alloc();
        pkt = av_packet_alloc();
    
        return frame && pkt;
    }
    
    bool FFmpegDecoder::decode(const uint8_t* data, size_t size, std::function<void(AVFrame*)> onFrame) {
        int ret = av_parser_parse2(parser, codecCtx, &pkt->data, &pkt->size, data, size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) return false;
    
        if (pkt->size > 0) {
            if (avcodec_send_packet(codecCtx, pkt) == 0) {
                while (avcodec_receive_frame(codecCtx, frame) == 0) {
                    onFrame(frame);
                }
            }
        }
        return true;
    }
    
    void FFmpegDecoder::cleanup() {
        av_packet_free(&pkt);
        av_frame_free(&frame);
        avcodec_free_context(&codecCtx);
        av_parser_close(parser);
    }

    AVFrame *FFmpegDecoder::decodeFrame(const std::vector<uint8_t> &data)
    {
        AVFrame *frame = av_frame_alloc();
        if (!frame) return nullptr;

        AVPacket *packet = av_packet_alloc();
        if (!packet) {
            av_frame_free(&frame);
            return nullptr;
        }

        packet->data = const_cast<uint8_t *>(data.data());
        packet->size = static_cast<int>(data.size());

        int ret = avcodec_send_packet(codecCtx, packet);
        if (ret < 0) {
            av_packet_free(&packet);
            return nullptr;
        }

        ret = avcodec_receive_frame(codecCtx, frame);
        av_packet_free(&packet);

        if (ret < 0) {
            av_frame_free(&frame);
            return nullptr;
        }

        return frame;
    }

    void FFmpegDecoder::releaseFrame(AVFrame *frame)
    {
        av_frame_free(&frame);
    }
