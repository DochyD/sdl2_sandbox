#include <iostream>
#include <stdexcept>

#include "sdl_utils.h"

// -- Constructor
SDLResources::SDLResources(){
    window = nullptr;
    renderer = nullptr;
    quit = NULL;
    windowWidth = NULL;
    windowHeight = NULL;
};

SDLResources::SDLResources(const char* title, const int width, const int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        SDL_Quit();
        throw std::runtime_error("SDL could set filtering! SDL_Error: " + std::string(SDL_GetError()));
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        SDL_Quit();
        throw std::runtime_error("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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
    quit = false;
    windowWidth = width;
    windowHeight = height;
    
    calculateViewportsPos();
}

// -- Destructor
SDLResources::~SDLResources() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = NULL;
    window = NULL;

	// Quit SDL subsystems
	//IMG_Quit();
    SDL_Quit();
}

// -- Event Handling
void SDLResources::processEvents(){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    // Handle window resize event if needed
                    windowWidth = event.window.data1;
				    windowHeight = event.window.data2;
                    // Flag to indicate the size changed
                    windowResized = true;
                }
                break;
        }
    }
}

void SDLResources::screenToGrid(int mouseX, int mouseY, int &gridX, int &gridY) {
    int tempX = mouseX - 400;  // Adjust based on your grid position
    int tempY = mouseY - 100;  // Adjust based on your grid position

    // Convert screen coordinates to isometric grid coordinates
    // gridX = (tempY / TILE_HEIGHT) + (tempX / TILE_WIDTH);
    // gridY = (tempY / TILE_HEIGHT) - (tempX / TILE_WIDTH);
}


// -- Window managment
void SDLResources::clear(){
    SDL_RenderClear(renderer);
}

void SDLResources::present() {
    SDL_RenderPresent(renderer);
}

void SDLResources::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}


// -- Drawing primitives
void SDLResources::drawRect(const SDL_Rect& rect) {
    SDL_RenderDrawRect(renderer, &rect);
}

void SDLResources::fillRect(const SDL_Rect& rect) {
    SDL_RenderFillRect(renderer, &rect);
}

void SDLResources::drawLine(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void SDLResources::drawDiamond(int x, int y, int h, int w) {
    SDL_Point points[5] = {
        {x - w/2, y},       // Top point
        {x, y + h/2},       // Right point
        {x - w/2, y + h},   // Bottom point
        {x - w, y + h/2},   // Left point
        {x - w/2, y}       // Back to top (to close the shape)
    };

    SDL_RenderDrawLines(renderer, points, 5);  // Draw the diamond shape
}

// -- Global Render function
void SDLResources::render(){
    renderViewports();
}

// -- Viewports functions
// Render all viewports
void SDLResources::renderViewports(){
    if (windowResized){
        calculateViewportsPos();
        windowResized = false;
    }

    renderMainViewport();
    renderBottomViewport();
    renderLeftViewport();
    renderRightViewport();
}

// Render a single viewport
void SDLResources::renderViewport(int idx, int r, int g, int b){
    // idx 0 = main / 1 = bottom / 2 = left / 3 = right
    SDL_RenderSetViewport(renderer, &viewports[idx]);
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, NULL);
}

// Calculate viewports positions based on window size.
void SDLResources::calculateViewportsPos(){
    // Main viewport (centered)
    viewports[0] = { 
        static_cast<int>(std::round(0.15 * windowWidth)), 
        0, 
        static_cast<int>(std::round(0.70 * windowWidth)), 
        static_cast<int>(std::round(0.80 * windowHeight)) 
    };

    // Bottom viewport
    viewports[1] = { 
        static_cast<int>(std::round(0.15 * windowWidth)), 
        static_cast<int>(std::round(0.80 * windowHeight)), 
        static_cast<int>(std::round(0.70 * windowWidth)), 
        static_cast<int>(std::round(0.20 * windowHeight)) 
    };

    // Left viewport
    viewports[2] = { 
        0, 
        0, 
        static_cast<int>(std::round(0.15 * windowWidth)), 
        windowHeight 
    };

    // Right viewport
    viewports[3] = { 
        static_cast<int>(std::round(0.85 * windowWidth)), 
        0, 
        static_cast<int>(std::round(0.15 * windowWidth)), 
        windowHeight 
    };
}

void SDLResources::renderMainViewport(){
    renderViewport(0, 35, 35, 35);
    drawIsometricGrid();
}

void SDLResources::renderBottomViewport(){
    renderViewport(1, 255, 0, 0);
}

void SDLResources::renderLeftViewport(){
    renderViewport(2, 0, 0, 0);
}

void SDLResources::renderRightViewport(){
    renderViewport(3, 0, 0, 0);
}



// Drawing the grid
void SDLResources::drawIsometricGrid() {
    // Grid size
    int gridWidth = 15; //15
    int gridHeight = 19; //19

    // Isometric ratio
    float isoRatio = 2.0f;

    // Get viewport dimensions
    int viewportWidth = viewports[0].w;
    int viewportHeight = viewports[0].h;

    // Calculate grid and viewport ratio
    float gridRatio = float(gridWidth) / (gridHeight);
    float viewportRatio = float(viewportWidth) / (viewportHeight * isoRatio);

    float tileWidth, tileHeight;

    // See if we want to calculate the tile width or height 
    if (viewportRatio <= gridRatio){
        tileWidth = (float)viewportWidth / gridWidth;
        tileHeight = tileWidth / isoRatio;
    }
    else{
        tileHeight = (float)viewportHeight / gridHeight;
        tileWidth = tileHeight * isoRatio;
    }

    // Apply an offset so the gris is always centered
    float x_offset = (viewportWidth - (tileWidth * gridWidth)) / 2;
    float y_offset = (viewportHeight - (tileHeight * gridHeight)) / 2;

    // Draw the grid
    for (int row = gridWidth; row > 0; --row) {
        for (int col = 0; col < gridHeight; ++col) {
            // Calculate isometric coordinates
            int x = (tileWidth * row) + x_offset;
            int y = (tileHeight* col) + y_offset;

            // Set tile color
            if ((row + col) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
            } else {
                SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 0xFF);
            }

            // Draw diamond
            drawDiamond(x, y, tileHeight, tileWidth);
        }
    }
}

