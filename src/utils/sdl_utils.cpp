#include <iostream>
#include <stdexcept>

#include "json_utils.h"
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
        SDL_Quit();
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
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    renderer = NULL;
    window = NULL;

	// Quit SDL subsystems
	//IMG_Quit();
    SDL_Quit();
}

// Setters
void SDLResources::loadMap(std::string filename){
    // Get reference to the vector2D inside IsometricGrid object.
    JsonUtils::loadGridFromJson(isometricGrid, filename);
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

void SDLResources::drawDiamond(int x, int y, int w, int h) {
    SDL_Point points[5] = {
        {x, y},       // Top point
        {x + w/2, y + h/2},       // Right point
        {x, y + h},   // Bottom point
        {x - w/2, y + h/2},   // Left point
        {x, y},      // Back to top (to close the shape)
    };

    SDL_RenderDrawLines(renderer, points, 5);  // Draw the diamond shape
}

void SDLResources::drawFilledDiamond(int x, int y, int w, int h) {
    SDL_Point points[5] = {
        {x, y},       // Top point
        {x + w/2, y + h/2},       // Right point
        {x, y + h},   // Bottom point
        {x - w/2, y + h/2},   // Left point
        {x, y},      // Back to top (to close the shape)
    };

    int topX = x;
    int topY = y;
    int bottomX = x;
    int bottomY = y + h;
    int leftX = x - w/2;
    int rightX = x + w/2;

    // Render filled top triangle (from top to middle)
    for (int i = 0; i <= h/2; ++i) {
        int startX = topX + (i * (leftX - topX)) / (h/2);
        int endX = topX + (i * (rightX - topX)) / (h/2);
        SDL_RenderDrawLine(renderer, startX, topY + i, endX, topY + i);
    }

    // Render filled bottom triangle (from middle to bottom)
    for (int i = 0; i <= h/2; ++i) {
        int startX = bottomX + (i * (leftX - bottomX)) / (h/2);
        int endX = bottomX + (i * (rightX - bottomX)) / (h/2);
        SDL_RenderDrawLine(renderer, startX, bottomY - i, endX, bottomY - i);
    }

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
void SDLResources::renderViewportBackground(int idx, int r, int g, int b){
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
    renderViewportBackground(0, 35, 35, 35);
    
    // To see the grid, used to test the render
    // drawIsometricGrid();

    // Transform data from function above into an IsometricGrid object.
    drawIsometricGrid();
}

void SDLResources::renderBottomViewport(){
    renderViewportBackground(1, 255, 0, 0);
}

void SDLResources::renderLeftViewport(){
    renderViewportBackground(2, 0, 0, 0);
}

void SDLResources::renderRightViewport(){
    renderViewportBackground(3, 0, 0, 0);
}

void SDLResources::drawIsometricGrid(){
    std::vector<std::vector<GridCell>>& grid = isometricGrid.getGrid();
    float x, y, baseX, baseY;
    float cellWidth, cellHeight;
    
    const float baseCellWidth = isometricGrid.getCellWidth();
    const float baseCellHeight = isometricGrid.getCellHeight();   
    const float baseViewportWidth = isometricGrid.getViewportWidth();
    const float baseViewportHeight = isometricGrid.getViewportHeight();

    const float currentViewportWidth = viewports[0].w;
    const float currentViewportHeight = viewports[0].h;

    // if current viewport size != base viewport size, recalculate cell size
    if (currentViewportWidth != baseViewportWidth){
        std::cout << "---- 1" << std::endl;
        std::cout << "---- currentViewportWidth : " << currentViewportWidth << std::endl;
        std::cout << "---- baseViewportWidth : " << baseViewportWidth << std::endl;
        cellWidth = (baseCellWidth * currentViewportWidth) / baseViewportWidth;
        cellHeight = (baseCellHeight * currentViewportHeight) / baseViewportHeight;
    }
    else {
        std::cout << "---- 2" << std::endl;
        cellWidth = baseCellWidth;
        cellHeight = baseCellHeight;
    }

    std::cout << "---- drawIsometricGrid" << std::endl;

    
    std::cout << "pls : " << isometricGrid.getGrid()[0][14].x << std::endl;
    std::cout << "pls : " << cellWidth << std::endl;
    std::cout << "pls : " << cellHeight << std::endl;


    // It prints a different value here
    

    //Draw the grid
    for (int h = 0; h < isometricGrid.getCellHeight(); ++h) {
        for (int w = 0; w < isometricGrid.getWidth(); ++w) {
            // std::cout << "-" << std::endl;
            // std::cout << grid[h][w].cellType << std::endl;
            // std::cout << grid[0][14].cellType << std::endl;

            if (grid[h][w].cellType != CellType::NO_RENDER){

                baseX = grid[h][w].x;
                baseY = grid[h][w].y;
                
                x = (baseX * currentViewportWidth) / baseViewportWidth;
                y = (baseY * currentViewportHeight) / baseViewportHeight; 

                if (w % 2 == 0) {
                    SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
                } else {
                    SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 0xFF);
                }

                // Draw diamond
                drawFilledDiamond(x, y, cellWidth, cellHeight);

                // // Draw outline
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                drawDiamond(x, y, cellWidth, cellHeight);
            }
        }
    }
}

// Drawing the grid and save to bleh.json
void SDLResources::drawIsometricGridThenCreateGridObject() {
    // Get const info from IsometricGrid object
    float renderedGridWidth = isometricGrid.getRenderedGridWidth();
    float renderedGridHeight = isometricGrid.getRenderedGridHeight();
    float isoRatio = isometricGrid.getIsoRatio();
    
    
    // Get viewport dimensions
    int viewportWidth = viewports[0].w;
    int viewportHeight = viewports[0].h;

    // Calculate grid and viewport ratio
    float gridRatio = float(renderedGridWidth) / (renderedGridHeight);
    float viewportRatio = float(viewportWidth) / (viewportHeight * isoRatio);

    float cellWidth, cellHeight;

    // See if we want to calculate the tile width or height first
    // based on the ratio of our viewport
    // (So we can have extra space at the left/right or top/bottom)
    if (viewportRatio <= gridRatio){
        cellWidth = static_cast<float>(viewportWidth) / renderedGridWidth;
        cellWidth = round(cellWidth);
        cellHeight = cellWidth / isoRatio;
    }
    else{
        cellHeight = static_cast<float>(viewportHeight) / renderedGridHeight;
        cellHeight = round(cellHeight);
        cellWidth = cellHeight * isoRatio;
    }

    // Apply an offset so the gris is always centered
    float x_offset = (viewportWidth - (cellWidth * renderedGridWidth)) / 2;
    // Since the point x of our grid is at the center top, we need to offset
    // be half the witdh of a cell
    x_offset -= (cellWidth/2);
    float y_offset = (viewportHeight - (cellHeight * renderedGridHeight)) / 2;

    // We first draw the the Top side then the bottom side, left to right
    // If we have obstacles, it will prevent having texture on each other.

    const int gridWidth = 33;
    const int gridHeight = 33;

    int xGrid;
    int yGrid;
    std::vector<std::vector<GridCell>>& isoGrid = isometricGrid.getGrid();

    for (int line = renderedGridWidth; line > 0; --line) {
        // x index to fill the IsometricGrid object
        int xGrid = renderedGridWidth - line;
        
        // Number of grid to draw for each line
        int numberOfGridToDraw = ((renderedGridWidth - line) * 2) + 1;
        
        for (int grid = 0; grid < numberOfGridToDraw; ++grid) {
            // y index to fill the IsometricGrid object
            yGrid = grid + (14 - (numberOfGridToDraw / 2));
            
            // top point of cell on screen
            float x = (cellWidth * line) + ((cellWidth/2) * grid) + x_offset;
            float y = ((cellHeight/2) * grid) + y_offset;

            // Add the cell to the IsometricGrad object
            isoGrid[xGrid][yGrid].x = x;
            isoGrid[xGrid][yGrid].y = y;
            isoGrid[xGrid][yGrid].cellType = WALKABLE;

            if (grid % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
            } else {
                SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 0xFF);
            }

            // Draw diamond
            drawFilledDiamond(x, y, cellWidth, cellHeight);

            // Draw outline
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            drawDiamond(x, y, cellWidth, cellHeight);
        }
    }

    // Draw the grid bottom side.
    for (int line = 1; line < renderedGridHeight; ++line) {
        
        int numberOfGridToDraw;
        
        // Number of grid to draw isn't linear for each new line 
        // because we have a rectangle shape.
        // We need to compensate because height > width
        // Also the line starting from the corner was drawn by the loop above
        // hence the two "-1" in the following lines
        if (line <= (renderedGridHeight - renderedGridWidth)){
            numberOfGridToDraw = ((renderedGridWidth - 1) * 2) + 1;
        }
        else{
            numberOfGridToDraw = ((renderedGridHeight - line - 1) * 2) + 1;
        }

        // x index to fill the IsometricGrid object
        xGrid = line + renderedGridWidth - 1;

        for (int grid = 0; grid < numberOfGridToDraw; ++grid) {
            // y index to fill the IsometricGrid object
            yGrid = grid + 1;

            // Calculate isometric coordinates
            float x = (cellWidth/2 * grid) + cellWidth + x_offset;
            float y = (cellHeight/2 * grid) + (cellHeight * line) + y_offset;

            // Add the cell to the IsometricGrad object
            isoGrid[xGrid][yGrid].x = x;
            isoGrid[xGrid][yGrid].y = y;
            isoGrid[xGrid][yGrid].cellType = WALKABLE;

            // Alternate colors
            if (grid % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
            } else {
                SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 0xFF);
            }

            // Draw diamond
            drawFilledDiamond(x, y, cellWidth, cellHeight);

            // Draw outline
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            drawDiamond(x, y, cellWidth, cellHeight);
        }
    }


    std::cout << "-----" << std::endl;
    std::cout << isoGrid[0][0].x << std::endl;
    std::cout << cellHeight << std::endl;
    isometricGrid.setCellWidth(cellWidth);
    isometricGrid.setCellHeight(cellHeight);
    isometricGrid.setViewportWidth(viewportWidth);
    isometricGrid.setViewportHeight(viewportHeight);

    //To save the grid:
    if (JsonUtils::saveGridToJson(isometricGrid, "bleh.json")) 
    {
        std::cout << "Grid saved successfully!" << std::endl;
    }
}

void SDLResources::drawIsometricGridDefault() {
    // Get const info from IsometricGrid object
    float renderedGridWidth = isometricGrid.getRenderedGridWidth();
    float renderedGridHeight = isometricGrid.getRenderedGridHeight();
    float isoRatio = isometricGrid.getIsoRatio();

    // Get viewport dimensions
    int viewportWidth = viewports[0].w;
    int viewportHeight = viewports[0].h;

    // Calculate grid and viewport ratio
    float gridRatio = float(renderedGridWidth) / (renderedGridHeight);
    float viewportRatio = float(viewportWidth) / (viewportHeight * isoRatio);

    float cellWidth, cellHeight;

    // See if we want to calculate the tile width or height first
    // based on the ratio of our viewport
    if (viewportRatio <= gridRatio){
        cellWidth = static_cast<float>(viewportWidth) / renderedGridWidth;
        cellHeight = cellWidth / isoRatio;
    }
    else{
        cellHeight = static_cast<float>(viewportHeight) / renderedGridHeight;
        cellWidth = cellHeight * isoRatio;
    }

    // Apply an offset so the gris is always centered
    float x_offset = (viewportWidth - (cellWidth * renderedGridWidth)) / 2;
    float y_offset = (viewportHeight - (cellHeight * renderedGridHeight)) / 2;

    // We first draw the the Top side then the bottom side, left to right
    // If we have obstacles, it will prevent having texture on each other.

    // Draw the grid (Top side)
    for (int line = renderedGridWidth; line > 0; --line) {
        int numberOfGridToDraw = ((renderedGridWidth - line) * 2) + 1;
        for (int grid = 0; grid < numberOfGridToDraw; ++grid) {
            // Calculate isometric coordinates
            float x = (cellWidth * line) + ((cellWidth/2) * grid) + x_offset;
            float y = ((cellHeight/2) * grid) + y_offset;

            if (grid % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
            } else {
                SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 0xFF);
            }

            // Draw diamond
            drawFilledDiamond(x, y, cellWidth, cellHeight);

            // Draw outline
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            drawDiamond(x, y, cellWidth, cellHeight);
        }
    }

    // Draw the grid bottom side.
    for (int line = 1; line < renderedGridHeight; ++line) {
        
        int numberOfGridToDraw;
        
        // Number of grid to draw isn't linear because we have a rectangle shape.
        // We need to compensate because height > width
        // Also the line starting from the corner was drawn by the loop above
        // hence the two "-1" in the following lines 
        if (line <= (renderedGridHeight - renderedGridWidth)){
            numberOfGridToDraw = ((renderedGridWidth - 1) * 2) + 1;
        }
        else{
            numberOfGridToDraw = ((renderedGridHeight - line - 1) * 2) + 1;
            numberOfGridToDraw = ((renderedGridHeight - line - 1) * 2) + 1;
        }

        for (int grid = 0; grid < numberOfGridToDraw; ++grid) {
            // Calculate isometric coordinates
            float x = (cellWidth/2 * grid) + cellWidth + x_offset;
            float y = (cellHeight/2 * grid) + (cellHeight * line) + y_offset;

            // Alternate colors
            if (grid % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
            } else {
                SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 0xFF);
            }

            // Draw diamond
            drawFilledDiamond(x, y, cellWidth, cellHeight);

            // Draw outline
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            drawDiamond(x, y, cellWidth, cellHeight);
        }
    }
}

