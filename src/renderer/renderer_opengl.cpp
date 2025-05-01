extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>

}

#include "renderer_opengl.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_opengl.h>
// #include <GL/glew.h>



RendererOpenGL::RendererOpenGL(int w, int h){
        window = nullptr;
        glContext = nullptr;
        texture = 0;
        width = w;
        height = h;

        std::cout << "Initializing renderer..." << std::endl;

    }

RendererOpenGL::~RendererOpenGL(){
    cleanup();
    std::cout << "Cleaning up renderer..." << std::endl;
    if (texture) glDeleteTextures(1, &texture);
    if (glContext) SDL_GL_DeleteContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    }

    bool RendererOpenGL::init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return false;
        }
    
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
        window = SDL_CreateWindow("OpenGL Renderer",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
        if (!window) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            return false;
        }
    
        glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
            return false;
        }
    
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            return false;
        }
    
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
        return true;
    }

void RendererOpenGL::renderFrame(const AVFrame* frame) {
    if (!frame || !frame->data[0]) return;

    static struct SwsContext* swsCtx = nullptr;
    uint8_t* rgbData[1] = { nullptr };
    int rgbLinesize[1] = { 0 };

    if (frame->format == AV_PIX_FMT_YUV420P) {
        // Allocate RGB buffer
        int size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
        uint8_t* buffer = (uint8_t*)av_malloc(size * sizeof(uint8_t));
        
        av_image_fill_arrays(rgbData, rgbLinesize, buffer, 
                            AV_PIX_FMT_RGB24, frame->width, frame->height, 1);

        // Convert YUV to RGB
        swsCtx = sws_getCachedContext(swsCtx,
            frame->width, frame->height, (AVPixelFormat)frame->format,
            frame->width, frame->height, AV_PIX_FMT_RGB24,
            SWS_BILINEAR, nullptr, nullptr, nullptr);

        sws_scale(swsCtx, frame->data, frame->linesize, 0, frame->height,
                 rgbData, rgbLinesize);

        // Upload to texture
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                    frame->width, frame->height,
                    0, GL_RGB, GL_UNSIGNED_BYTE, rgbData[0]);
        
        av_free(buffer);
    } else {
        // Direct upload for RGB frames
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                    frame->width, frame->height,
                    0, GL_RGB, GL_UNSIGNED_BYTE, frame->data[0]);
    }

    // Render
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(-1, -1);
    glTexCoord2f(1, 1); glVertex2f(1, -1);
    glTexCoord2f(1, 0); glVertex2f(1, 1);
    glTexCoord2f(0, 0); glVertex2f(-1, 1);
    glEnd();

    SDL_GL_SwapWindow(window);
}

void RendererOpenGL::cleanup() {
    if (texture) glDeleteTextures(1, &texture);
    if (glContext) SDL_GL_DeleteContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool RendererOpenGL::shouldClose() {
    return !isRunning;
}

void RendererOpenGL::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            isRunning = false;
    }
}

void RendererOpenGL::render(const AVFrame* frame) {
    renderFrame(frame);
}