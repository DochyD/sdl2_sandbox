// C++ Standard Libraries
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "sdl_utils.h"


// Window name
const char* WINDOW_NAME = "SDL2 Tactics";

// Screen dimension constants
const float BASE_WINDOW_WIDTH = 1280;
const float BASE_WINDOW_HEIGHT = 720;

const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 15;
const int TILE_SIZE = 32;

// Define the viewport rects
SDL_Rect viewports[4] = {
	{0.15 * BASE_WINDOW_WIDTH, 0, 0.70 * BASE_WINDOW_WIDTH, 0.8 * BASE_WINDOW_HEIGHT},				// main
	{0.15 * BASE_WINDOW_WIDTH, 0.8 * BASE_WINDOW_HEIGHT, 0.70 * BASE_WINDOW_WIDTH, 0.2 * BASE_WINDOW_HEIGHT}, // bottom
	{0, 0, 0.15 * BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT},										// left
	{0.85 * BASE_WINDOW_WIDTH, 0, 0.15 * BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT}					// right
};

Uint8 backgroundColors[4][3] = {
	{255, 0, 0},  // Red
	{0, 255, 0},  // Green
	{0, 0, 255},  // Blue
	{255, 255, 0} // Yellow
};

// Starts up SDL and creates window
bool init();


int main(int argc, char *args[])
{
	float scrn_width = BASE_WINDOW_WIDTH;
	float scrn_height = BASE_WINDOW_WIDTH;

	SDLResources sdl;

	// Initialize SDL
	try {
        sdl = initSDL(WINDOW_NAME, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT);
		// TODO : Load media if needed here
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

	// Main loop
	while (!sdl.quit)
	{
		processEvents(sdl);

		// Clear screen
		SDL_SetRenderDrawColor(sdl.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(sdl.renderer);

		// Render the grid in each viewport
		for (int i = 0; i < 4; i++)
		{
			SDL_RenderSetViewport(sdl.renderer, &viewports[i]);
			SDL_SetRenderDrawColor(sdl.renderer, backgroundColors[i][0], backgroundColors[i][1], backgroundColors[i][2], 255);
			SDL_RenderFillRect(sdl.renderer, NULL);
		}

		SDL_RenderPresent(sdl.renderer);
	}

	cleanupSDL(sdl);

	return 0;
}
