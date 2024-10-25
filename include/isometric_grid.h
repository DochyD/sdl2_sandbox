// In a new header file: isometric_grid.h
#pragma once
#include <vector>
#include <memory>
#include "grid_cell.h"

class IsometricGrid {
private:
    // 2D Vector + it's size 
    std::vector<std::vector<GridCell>> grid;
    const int gridWidth = 33;
    const int gridHeight = 33;

    // Max number of cell in width and height that we can render on screen
    const int renderedGridWidth = 15;
    const int renderedGridHeight = 19;

    // Ratio width:height (2:1) to render a cell
    const float isoRatio = 2.0f;
    float cellWidth;
    float cellHeight;

    // viewport dimension when grid was created
    float viewportWidth;
    float viewportHeight;

    // Convert screen coordinates to grid coordinates
    bool isValidPosition(int x, int y) const {
        return x >= 0 && x < gridWidth && y >= 0 && y < gridHeight;
    }

public:
    IsometricGrid() {
        // Initialize the grid with 33x33 GridCell objects
        grid.resize(gridHeight, std::vector<GridCell>(gridWidth));

        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                grid[x][y] = GridCell();
            }
        }
    }

    // Getters
    std::vector<std::vector<GridCell>>& getGrid() { return grid; }// reference 
    const std::vector<std::vector<GridCell>>& getGrid() const { return grid; }// reference 
    int getWidth() const { return gridWidth; }
    int getHeight() const { return gridHeight; }    
    int getRenderedGridWidth() const { return renderedGridWidth; }
    int getRenderedGridHeight() const { return renderedGridHeight; }    
    float getViewportWidth() const { return viewportWidth; }
    float getViewportHeight() const { return viewportHeight; }
    float getCellWidth() const { return cellWidth; }
    float getCellHeight() const { return cellHeight; }
    float getIsoRatio() const { return isoRatio; }

    // Setters
    void setViewportWidth(float w) {viewportWidth = w;}
    void setViewportHeight(float h) {viewportHeight = h;}
    void setCellWidth(float w) {cellWidth = w;}
    void setCellHeight(float h) {cellHeight = h;}
    void setGridCell(size_t row, size_t col, const GridCell& newCell) {
        grid[row][col] = newCell;  // Modify a specific cell in the grid
    }
    
    

    // Get cell at grid coordinates
    GridCell* getCell(int x, int y) {
        if (!isValidPosition(x, y)) return nullptr;
        return &grid[x][y];
    }

    // Convert screen coordinates to grid coordinates
    bool screenToGrid(float screenX, float screenY, float tileWidth, float tileHeight, 
                     float offsetX, float offsetY, int& gridX, int& gridY) {
        // Adjust for offset
        screenX -= offsetX;
        screenY -= offsetY;

        // Convert screen coordinates to grid coordinates using isometric formula
        float isoX = (screenX / (tileWidth / 2) + screenY / (tileHeight / 2)) / 2;
        float isoY = (screenY / (tileHeight / 2) - screenX / (tileWidth / 2)) / 2;

        // Round to nearest grid coordinates
        gridX = static_cast<int>(std::round(isoX));
        gridY = static_cast<int>(std::round(isoY));

        return isValidPosition(gridX, gridY);
    }

    // Convert grid coordinates to screen coordinates
    void gridToScreen(int gridX, int gridY, float tileWidth, float tileHeight,
                     float offsetX, float offsetY, float& screenX, float& screenY) {
        // Convert grid coordinates to screen coordinates using isometric formula
        screenX = (gridX - gridY) * (tileWidth / 2) + offsetX;
        screenY = (gridX + gridY) * (tileHeight / 2) + offsetY;
    }

    void setCellType(int x, int y, const CellType type) {
        if (GridCell* cell = getCell(x, y)) {
            cell->cellType = type;
        }
    }
};
