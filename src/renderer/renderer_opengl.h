#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <libavutil/frame.h>

class RendererOpenGL {
public:
    RendererOpenGL(int width, int height);
    ~RendererOpenGL();

    bool init(int argc, char* argv[]);  // fix to match implementation
    void renderFrame(const AVFrame* frame); // Or use D3D texture if you are not using AVFrame
    void cleanup();
    
    bool shouldClose();   // Declare this if used
    void pollEvents();    // Declare this if used
    void render(const AVFrame* frame); // Match the call in main

private:
    SDL_Window* window;
    SDL_GLContext glContext;
    int width, height;
    GLuint texture;
    bool isRunning = true; // for shouldClose()
};
