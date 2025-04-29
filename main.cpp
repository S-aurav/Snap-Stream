#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <SDL2/SDL.h>
#include <iostream>

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Unable to open X display.\n";
        return 1;
    }

    Window root = DefaultRootWindow(display);

    // Get screen dimensions
    int screen_num = DefaultScreen(display);
    // int width = DisplayWidth(display, screen_num);
    // int height = DisplayHeight(display, screen_num);

    int width = 1280;
    int height = 720;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Screen Capture",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING,
        width, height);

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        XImage* img = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);

        // Copy image to SDL texture
        void* pixels;
        int pitch;
        SDL_LockTexture(texture, nullptr, &pixels, &pitch);

        // Note: Assumes 32-bit image (RGBA or ARGB)
        memcpy(pixels, img->data, height * pitch);

        SDL_UnlockTexture(texture);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        XDestroyImage(img);
        SDL_Delay(1); // ~60 fps
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    XCloseDisplay(display);

    return 0;
}
