// encoder_nvenc.cpp
#include "encoder_nvenc.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

static AVCodecContext* enc_ctx = nullptr;
static AVFrame* frame = nullptr;
static AVPacket* pkt = nullptr;

bool init_encoder(int width, int height) {
    // avcodec_register_all();
    const AVCodec* codec = avcodec_find_encoder_by_name("h264_nvenc");
    if (!codec) return false;

    enc_ctx = avcodec_alloc_context3(codec);
    enc_ctx->width = width;
    enc_ctx->height = height;
    enc_ctx->time_base = {1, 60};
    enc_ctx->framerate = {60, 1};
    enc_ctx->pix_fmt = AV_PIX_FMT_NV12;
    enc_ctx->bit_rate = 4000000;
    av_opt_set(enc_ctx->priv_data, "preset", "llhq", 0);

    if (avcodec_open2(enc_ctx, codec, nullptr) < 0)
        return false;

    frame = av_frame_alloc();
    frame->format = enc_ctx->pix_fmt;
    frame->width = enc_ctx->width;
    frame->height = enc_ctx->height;
    av_frame_get_buffer(frame, 32);

    pkt = av_packet_alloc();
    return true;
}

bool encode_frame(ID3D11Texture2D* texture, std::vector<uint8_t>& outPacket) {
    // Map texture and copy data to frame->data
    // For simplicity, this part is omitted

    frame->pts++;
    if (avcodec_send_frame(enc_ctx, frame) < 0) return false;
    if (avcodec_receive_packet(enc_ctx, pkt) == 0) {
        outPacket.assign(pkt->data, pkt->data + pkt->size);
        av_packet_unref(pkt);
        return true;
    }
    return false;
}

void close_encoder() {
    avcodec_free_context(&enc_ctx);
    av_frame_free(&frame);
    av_packet_free(&pkt);
}
