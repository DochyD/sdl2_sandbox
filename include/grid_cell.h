#pragma once

enum CellType {
  WALKABLE,
  EMPTY,
  OBSTACLE,
  NO_RENDER
}; 

struct GridCell {
    // Cell coordinates on screen for the base viewport
    // represents the to point of the cell
    float x; 
    float y;
    CellType cellType;
    bool occupied = false; // is there a character on it?
    
    GridCell(){
      this->x = 0;
      this->y = 0;
      this->occupied = false;
      this->cellType = NO_RENDER;
    }
    GridCell(float x, float y, CellType celltype, bool occupied){
      this->x = x;
      this->y = y;
      this->occupied = occupied;
      this->cellType = celltype;
    }
};