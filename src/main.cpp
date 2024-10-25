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

int main(int argc, char *args[])
{

	SDLResources sdl(WINDOW_NAME, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT);
	
	// load map
	sdl.loadMap("test_map.json");

	
	// Main loop
	while (!sdl.getQuit())
	{
		// Handle events
		sdl.processEvents();

		// Game Logic
		// TODO

		// Clear the renderer with white background
		sdl.setDrawColor(255, 255, 255, 255);
		sdl.clear();

		// Rendering
		sdl.render();
		
		// Update the screen
		sdl.present();
	}
	
	return 0;
}
