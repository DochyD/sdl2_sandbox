#include <iostream>
#include <stdexcept>

#include "sdl_utils.h"

SDLResources initSDL(const char* title, const int width, const int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        SDL_Quit();
        throw std::runtime_error("SDL could set filtering! SDL_Error: " + std::string(SDL_GetError()));
    }

    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        SDL_Quit();
        throw std::runtime_error("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()));
    }

    // // Initialize renderer color
    // SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    
    // // Initialize PNG loading
    // int imgFlags = IMG_INIT_PNG;
    // if (!(IMG_Init(imgFlags) & imgFlags))
    // {
    //     SDL_DestroyRenderer(renderer);
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();
    //     throw std::runtime_error("SDL2's PNG loading init failed! SDL_Error: " + std::string(SDL_GetError()));
    // }

    return {window, renderer, false, width, height};
}

void cleanupSDL(SDLResources& resources) {
    SDL_DestroyRenderer(resources.renderer);
    SDL_DestroyWindow(resources.window);
    resources.renderer = NULL;
    resources.window = NULL;

	// Quit SDL subsystems
	//IMG_Quit();
    SDL_Quit();
}

void processEvents(SDLResources& resources){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        
        switch (event.type) {
            case SDL_QUIT:
                resources.quit = true;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    // Handle window resize event if needed
                    std::cout << "test" << std::endl;
                    resources.windowWidth = event.window.data1;
				    resources.windowHeight = event.window.data2;
                }
                break;
        }
    }
}