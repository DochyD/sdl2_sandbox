#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <memory>

#include "isometric_grid.h"

class SDLResources {

    private:
        // var
        SDL_Window* window;
        SDL_Renderer* renderer;
        int windowWidth;
        int windowHeight;
        bool quit;
        bool windowResized = false;

        // viewports data
        SDL_Rect viewports[4];

        // grid data
        IsometricGrid isometricGrid;

    public:
        // Constructor / Destructor
        SDLResources();
        SDLResources(const char* title, int width, int height);
        ~SDLResources();

        // Delete copy constructor and assignment operator
        SDLResources(const SDLResources&) = delete;
        SDLResources& operator=(const SDLResources&) = delete;

        // Getters
        SDL_Renderer* getRenderer() const { return renderer; }
        SDL_Window* getWindow() const { return window; }
        bool getQuit() { return quit; }
        int getWindowWidth() const { return windowWidth; }
        int getWindowHeight() const { return windowHeight; }

        // Setters
        void setQuit(bool b) { quit =b; }
        void loadMap(std::string filename); // setGrid() equivalent

        // Event Handling
        void processEvents();
        void screenToGrid(int mouseX, int mouseY, int &gridX, int &gridY);

        // Window management
        void clear();
        void present();
        void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        void toggleFullscreen();
        void setWindowSize(int width, int height);

        // Drawing primitives
        void drawRect(const SDL_Rect& rect);
        void fillRect(const SDL_Rect& rect);
        void drawLine(int x1, int y1, int x2, int y2);
        void drawDiamond(int x, int y, int h, int w);
        void drawFilledDiamond(int x, int y, int h, int w);

        // Global Render function
        void render();

        // Viewports functions
        void calculateViewportsPos();
        void renderViewports();
        void renderViewportBackground(int idx, int r, int g, int b);
        void renderLeftViewport();
        void renderRightViewport();
        void renderMainViewport();
        void renderBottomViewport();

        // Grid (Main viewport)
        // Used to experiemente what how many grids I'll have on the screen
        void drawIsometricGridDefault(); 
        // Used to convert what I have on my screen to an array containing info
        void drawIsometricGridThenCreateGridObject();
        // Draw grid from 2d vector
        void drawIsometricGrid();

};

#endif // SDL_UTILS_H
 