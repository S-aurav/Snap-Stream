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
        if (!codec) {
            std::cerr << "H.264 decoder not found" << std::endl;
            return false;
        }
    
        codecCtx = avcodec_alloc_context3(codec);
        if (!codecCtx) return false;
    
        // Critical decoder settings
        codecCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;
        codecCtx->flags2 |= AV_CODEC_FLAG2_CHUNKS | AV_CODEC_FLAG2_FAST;
        codecCtx->thread_count = 0; // Auto-detect
        codecCtx->thread_type = FF_THREAD_FRAME;
        codecCtx->err_recognition = AV_EF_CAREFUL | AV_EF_COMPLIANT;
    
        if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
            std::cerr << "Failed to open decoder" << std::endl;
            return false;
        }
    
        parser = av_parser_init(codec->id);
        if (!parser) {
            std::cerr << "Failed to initialize parser" << std::endl;
            return false;
        }
    
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

    AVFrame* FFmpegDecoder::decodeFrame(const std::vector<uint8_t>& data) {

        if (data.empty()) return nullptr;

        // Ensure we have a start code
        const uint8_t start_code[] = {0,0,0,1};
        bool has_start_code = data.size() > 4 && 
                            memcmp(data.data(), start_code, 4) == 0;

        AVPacket* packet = av_packet_alloc();
        if (!has_start_code) {
            // Insert start code if missing
            packet->size = data.size() + 4;
            packet->data = (uint8_t*)av_malloc(packet->size);
            memcpy(packet->data, start_code, 4);
            memcpy(packet->data+4, data.data(), data.size());
        } else {
            packet->data = const_cast<uint8_t*>(data.data());
            packet->size = data.size();
        }

        if (!codecCtx || data.empty()) return nullptr;
    
        // AVPacket* packet = av_packet_alloc();
        if (!packet) return nullptr;
    
        // Handle potential missing start code
        if (data.size() > 3 && data[0] == 0 && data[1] == 0 && data[2] == 1) {
            packet->data = const_cast<uint8_t*>(data.data());
            packet->size = static_cast<int>(data.size());
        } else {
            std::vector<uint8_t> modifiedData;
            modifiedData.insert(modifiedData.end(), {0, 0, 0, 1});
            modifiedData.insert(modifiedData.end(), data.begin(), data.end());
            packet->data = modifiedData.data();
            packet->size = static_cast<int>(modifiedData.size());
        }
    
        int ret = avcodec_send_packet(codecCtx, packet);
        av_packet_free(&packet);
        
        if (ret < 0) {
            std::cerr << "Error sending packet: " << av_err2str(ret) << std::endl;
            return nullptr;
        }
    
        AVFrame* frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecCtx, frame);
        
        if (ret < 0) {
            av_frame_free(&frame);
            if (ret != AVERROR(EAGAIN)) {
                std::cerr << "Error receiving frame: " << av_err2str(ret) << std::endl;
            }
            return nullptr;
        }
    
        return frame;
    }
    

    void FFmpegDecoder::releaseFrame(AVFrame *frame)
    {
        av_frame_free(&frame);
    }
