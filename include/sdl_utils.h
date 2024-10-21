#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct SDLResources {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool quit;
    int windowWidth;
    int windowHeight;
};

void cleanupSDL(SDLResources& resources);
SDLResources initSDL(const char* title, int width, int height);
void processEvents(SDLResources& resources);

#endif // SDL_UTILS_H