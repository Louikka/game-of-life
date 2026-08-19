#ifndef GAME_OF_LIFE_GRID_H
#define GAME_OF_LIFE_GRID_H



#define CELL_SIZE 10 // In pixels.


/** `DEAD` = 0, `ALIVE` = 1. */
typedef char CellState;

// two possible states of cell
#define DEAD 0
#define ALIVE 1


/**
 * Game grid dimension (width).
 */
extern int GAME_GRID_WIDTH;
/**
 * Game grid dimension (height).
 */
extern int GAME_GRID_HEIGHT;
/**
 * Total amount of cells in grid.
 */
extern int TOTAL_GRID_SIZE;


/**
 * @param `i` index in the array.
 */
SDL_Point GetCellPosition(int i);

/**
 * Returns `SDL_Point` struct with positions adjusted with corresponded parameters.
 */
SDL_Point GetCellPositionAdj(int i, int x, int y);


CellState GetCellState(CellState *grid, SDL_Point pos);

void SetCellState(CellState *grid, SDL_Point pos, CellState State);

bool IsCellAlive(CellState *grid, SDL_Point pos);

void ToggleCellState(CellState *grid, SDL_Point pos);


void UpdateGrid(CellState *Current, CellState *Next);

void ClearGrid(CellState *grid);



#endif
