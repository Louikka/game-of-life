#include <SDL3/SDL.h>

#include "game_grid.h"



int GameGridWidth = 0;
int GameGridHeight = 0;

int CalculateGameGridDimensions(const SDL_Rect WindowRect)
{
    GameGridWidth = WindowRect.w / CELL_SIZE;
    GameGridHeight = WindowRect.h / CELL_SIZE;

    int TotalGridSize = 0;

    if (GameGridWidth <= 0 || GameGridHeight <= 0)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Failed to figure out the game grid size.\n"
        );
    }
    else
    {
        TotalGridSize = GameGridWidth * GameGridHeight;

        SDL_Log(
            "Size of the game world : %dx%d. Total amount of cells in grid : %d\n",
            GameGridWidth, GameGridHeight,
            TotalGridSize
        );
    }

    return TotalGridSize;
}


CellState *GameGrid_Current = NULL;
CellState *GameGrid_Next = NULL;

static void _FreeGrid(void)
{
    free(GameGrid_Current);
    GameGrid_Current = NULL;

    free(GameGrid_Next);
    GameGrid_Next = NULL;
}

bool InitGameGrid()
{
    const int TOTAL_GRID_SIZE = GameGridWidth * GameGridHeight;
    if (TOTAL_GRID_SIZE <= 0)
    {
        return false;
    }

    GameGrid_Current = (CellState *)calloc(TOTAL_GRID_SIZE, sizeof(CellState));
    GameGrid_Next = (CellState *)calloc(TOTAL_GRID_SIZE, sizeof(CellState));

    if (GameGrid_Current == NULL || GameGrid_Next == NULL)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Failed to allocate memory for game grids.\n"
        );

        return false;
    }
    else
    {
        atexit(_FreeGrid);
    }

    return true;
}



SDL_Point GetCellPosition(int i)
{
    return (SDL_Point){
        .x = (i % GameGridWidth),
        .y = (int)(i / GameGridWidth),
    };
}

SDL_Point GetCellPositionAdj(int i, int x, int y)
{
    return (SDL_Point){
        .x = (i % GameGridWidth) + x,
        .y = (int)(i / GameGridWidth) + y,
    };
}


CellState GetCellState(CellState *grid, SDL_Point pos)
{
    if (pos.x < 0 || pos.x >= GameGridWidth)
    {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Trying to get state of cell outside of the grid (%d, %d).",
            pos.x,
            pos.y
        );

        return DEAD;
    }

    if (pos.y < 0 || pos.y >= GameGridHeight)
    {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Trying to get state of cell outside of the grid (%d, %d).",
            pos.x,
            pos.y
        );

        return DEAD;
    }

    return grid[pos.y * GameGridWidth + pos.x];
}

void SetCellState(CellState *grid, SDL_Point pos, CellState State)
{
    if (pos.x < 0 || pos.x >= GameGridWidth)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to set state of the cell : cell outside of the grid."
        );

        return;
    }

    if (pos.y < 0 || pos.y >= GameGridHeight)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to set state of the cell : cell outside of the grid."
        );

        return;
    }

    grid[pos.y * GameGridWidth + pos.x] = State;
}

bool IsCellAlive(CellState *grid, SDL_Point pos)
{
    return GetCellState(grid, pos) == ALIVE;
}

void ToggleCellState(CellState *grid, SDL_Point pos)
{
    if (IsCellAlive(grid, pos))
    {
        SetCellState(grid, pos, DEAD);
    }
    else
    {
        SetCellState(grid, pos, ALIVE);
    }
}



void UpdateGrid(CellState *Current, CellState *Next)
{
    const int TOTAL_GRID_SIZE = GameGridWidth * GameGridHeight;

    for (int i = 0; i < TOTAL_GRID_SIZE; i++)
    {
        const SDL_Point Cell = GetCellPosition(i);

        unsigned int Neighbours = 0;

        //
        //   o
        // x
        if (
            Cell.x >= 1
            && Cell.y <= (GameGridHeight - 2)
            && IsCellAlive(Current, GetCellPositionAdj(i, -1, 1))
        ) Neighbours++;

        //
        //   o
        //   x
        if (
            Cell.y <= (GameGridHeight - 2)
            && IsCellAlive(Current, GetCellPositionAdj(i, 0, 1))
        ) Neighbours++;

        //
        //   o
        //     x
        if (
            Cell.x <= (GameGridWidth - 2)
            && Cell.y <= (GameGridHeight - 2)
            && IsCellAlive(Current, GetCellPositionAdj(i, 1, 1))
        ) Neighbours++;

        //
        // x o
        //
        if (
            Cell.x >= 1
            && IsCellAlive(Current, GetCellPositionAdj(i, -1, 0))
        ) Neighbours++;

        //
        //   o x
        //
        if (
            Cell.x <= (GameGridWidth - 2)
            && IsCellAlive(Current, GetCellPositionAdj(i, 1, 0))
        ) Neighbours++;

        // x
        //   o
        //
        if (
            Cell.x >= 1
            && Cell.y >= 1
            && IsCellAlive(Current, GetCellPositionAdj(i, -1, -1))
        ) Neighbours++;

        //   x
        //   o
        //
        if (
            Cell.y >= 1
            && IsCellAlive(Current, GetCellPositionAdj(i,  0, -1))
        ) Neighbours++;

        //     x
        //   o
        //
        if (
            Cell.x <= (GameGridWidth - 2)
            && Cell.y >= 1
            && IsCellAlive(Current, GetCellPositionAdj(i,  1, -1))
        ) Neighbours++;


        if ((!IsCellAlive(Current, Cell) && Neighbours == 3)
            || (IsCellAlive(Current, Cell) && (Neighbours == 2 || Neighbours == 3)))
        {
            SetCellState(Next, Cell, ALIVE);
        }
        else
        {
            SetCellState(Next, Cell, DEAD);
        }
    }

    // copy next generation grid to the current grid
    for (int i = 0; i < TOTAL_GRID_SIZE; i++)
    {
        const SDL_Point Cell = GetCellPosition(i);

        SetCellState(
            Current,
            Cell,
            GetCellState(Next, Cell)
        );
        SetCellState(Next, Cell, DEAD);
    }
}

void ClearGrid(CellState *grid)
{
    const int TOTAL_GRID_SIZE = GameGridWidth * GameGridHeight;

    for (int i = 0; i < TOTAL_GRID_SIZE; i++)
    {
        const SDL_Point Cell = GetCellPosition(i);
        SetCellState(grid, Cell, DEAD);
    }
}
