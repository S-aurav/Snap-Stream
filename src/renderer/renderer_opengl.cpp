#include "renderer_opengl.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_opengl.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
}

RendererOpenGL::RendererOpenGL(int w, int h)
    : window(nullptr), glContext(nullptr), width(w), height(h), texture(0) {
        std::cout << "Initializing renderer..." << std::endl;

    }

RendererOpenGL::~RendererOpenGL()
{
}

bool RendererOpenGL::init(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("OpenGL Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        return false;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    return true;
}

void RendererOpenGL::renderFrame(const AVFrame* frame) {
    if (!frame || !frame->data[0]) return;

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, frame->data[0]);

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-1.0f,  1.0f);
    glTexCoord2f(1, 0); glVertex2f( 1.0f,  1.0f);
    glTexCoord2f(1, 1); glVertex2f( 1.0f, -1.0f);
    glTexCoord2f(0, 1); glVertex2f(-1.0f, -1.0f);
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