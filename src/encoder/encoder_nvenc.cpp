// encoder_nvenc.cpp
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
}
#include "encoder_nvenc.h"
#include <iostream>
#include <fstream>


static AVCodecContext* enc_ctx = nullptr;
static AVFrame* frame = nullptr;
static AVPacket* pkt = nullptr;

bool init_encoder(int width, int height) {
    const AVCodec* codec = avcodec_find_encoder_by_name("h264_nvenc");
    if (!codec) {
        std::cerr << "h264_nvenc encoder not found" << std::endl;
        return false;
    }

    enc_ctx = avcodec_alloc_context3(codec);
    enc_ctx->width = width;
    enc_ctx->height = height;
    enc_ctx->time_base = {1, 60};
    enc_ctx->framerate = {60, 1};
    enc_ctx->pix_fmt = AV_PIX_FMT_NV12;
    enc_ctx->bit_rate = 4000000;
    enc_ctx->gop_size = 10;
    enc_ctx->max_b_frames = 0;
    enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    enc_ctx->strict_std_compliance = FF_COMPLIANCE_STRICT;

    // Critical: Set profile to high for better compatibility
    av_opt_set(enc_ctx->priv_data, "profile", "high", 0);
    av_opt_set(enc_ctx->priv_data, "preset", "llhp", 0);
    av_opt_set(enc_ctx->priv_data, "forced-idr", "1", 0);
    av_opt_set(enc_ctx->priv_data, "delay", "0", 0);
    av_opt_set(enc_ctx->priv_data, "repeat-sps-pps", "1", 0);

    if (avcodec_open2(enc_ctx, codec, nullptr) < 0) {
        std::cerr << "Failed to open encoder" << std::endl;
        return false;
    }

    frame = av_frame_alloc();
    frame->format = enc_ctx->pix_fmt;
    frame->width = enc_ctx->width;
    frame->height = enc_ctx->height;
    if (av_frame_get_buffer(frame, 0) < 0) {
        std::cerr << "Failed to allocate frame" << std::endl;
        return false;
    }

    pkt = av_packet_alloc();
    return true;
}

bool encode_frame(ID3D11Texture2D* texture, std::vector<uint8_t>& outPacket) {
    
    static std::ofstream testFile("test.h264", std::ios::binary);
    static bool firstFrame = true;

    if (firstFrame) {
        // Write SPS/PPS to test file
        if (enc_ctx->extradata && enc_ctx->extradata_size > 0) {
            testFile.write(reinterpret_cast<const char*>(enc_ctx->extradata), 
                         enc_ctx->extradata_size);
        }
        firstFrame = false;
    }else{
        std::cout<<"Encoder is not ready yet!"<<std::endl;
        // return false;
    }

    static int frameCount = 0;
    frame->pts = frameCount++;

    if (avcodec_send_frame(enc_ctx, frame) < 0) {
        std::cerr << "Error sending frame to encoder" << std::endl;
        return false;
    }

    outPacket.clear();
    
    while (true) {
        int ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
        if (ret < 0) return false;

        // Convert to Annex B format (with start codes)
        if (pkt->flags & AV_PKT_FLAG_KEY) {
            // Prepend SPS/PPS for keyframes
            const uint8_t start_code[] = {0, 0, 0, 1};
            outPacket.insert(outPacket.end(), start_code, start_code + 4);
            outPacket.insert(outPacket.end(), 
                           enc_ctx->extradata, 
                           enc_ctx->extradata + enc_ctx->extradata_size);
        }
        
        // Add start code before NAL unit
        const uint8_t start_code[] = {0, 0, 0, 1};
        outPacket.insert(outPacket.end(), start_code, start_code + 4);
        outPacket.insert(outPacket.end(), pkt->data, pkt->data + pkt->size);
        
        av_packet_unref(pkt);

    }

    if (avcodec_receive_packet(enc_ctx, pkt) == 0) {
        // Write to test file
        testFile.write(reinterpret_cast<const char*>(pkt->data), pkt->size);
        testFile.flush();
        
        outPacket.assign(pkt->data, pkt->data + pkt->size);
        av_packet_unref(pkt);
        return true;
    }

    return !outPacket.empty();
}

void close_encoder() {
    avcodec_free_context(&enc_ctx);
    av_frame_free(&frame);
    av_packet_free(&pkt);
}
