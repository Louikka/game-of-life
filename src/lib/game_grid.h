#ifndef GAME_OF_LIFE_GAME_GRID_H
#define GAME_OF_LIFE_GAME_GRID_H



#define CELL_SIZE 10 // In pixels.


/**
 * `DEAD` = 0, `ALIVE` = 1.
 */
typedef char CellState;

// two possible states of cell
#define DEAD 0
#define ALIVE 1


/**
 * Game grid dimension (width).
 */
extern int GameGridWidth;
/**
 * Game grid dimension (height).
 */
extern int GameGridHeight;

/**
 * @returns Total grid size as int, or 0 on failure.
 */
int CalculateGameGridDimensions(SDL_Rect WindowRect);


extern CellState *GameGrid_Current;
extern CellState *GameGrid_Next;

/**
 * @returns `true` on success, `false` on failure.
 */
bool InitGameGrid();



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
