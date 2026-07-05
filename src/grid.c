#include <SDL3/SDL.h>

#include "grid.h"



int GAME_GRID_WIDTH;
int GAME_GRID_HEIGHT;

int TOTAL_GRID_SIZE;



/**
 * @param `i` index in the array.
 */
SDL_Point GetCellPosition(int i)
{
    return (SDL_Point){
        .x = (i % GAME_GRID_WIDTH),
        .y = (int)(i / GAME_GRID_WIDTH),
    };
}
/** Returns `SDL_Point` struct with positions adjusted with corresponded parameters. */
SDL_Point GetCellPositionAdj(int i, int x, int y)
{
    return (SDL_Point){
        .x = (i % GAME_GRID_WIDTH) + x,
        .y = (int)(i / GAME_GRID_WIDTH) + y,
    };
}


CellState GetCellState(CellState *grid, SDL_Point pos)
{
    if (pos.x < 0 || pos.x >= GAME_GRID_WIDTH)
    {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Trying to get state of cell outside of the grid (%d, %d).",
            pos.x,
            pos.y
        );

        return DEAD;
    }

    if (pos.y < 0 || pos.y >= GAME_GRID_HEIGHT)
    {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Trying to get state of cell outside of the grid (%d, %d).",
            pos.x,
            pos.y
        );

        return DEAD;
    }

    return grid[pos.y * GAME_GRID_WIDTH + pos.x];
}

void SetCellState(CellState *grid, SDL_Point pos, CellState State)
{
    if (pos.x < 0 || pos.x >= GAME_GRID_WIDTH)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to set state of the cell : cell outside of the grid."
        );

        return;
    }

    if (pos.y < 0 || pos.y >= GAME_GRID_HEIGHT)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to set state of the cell : cell outside of the grid."
        );

        return;
    }

    grid[pos.y * GAME_GRID_WIDTH + pos.x] = State;
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
    for (int i = 0; i < TOTAL_GRID_SIZE; i++)
    {
        const SDL_Point Cell = GetCellPosition(i);

        unsigned int Neighbours = 0;

        //
        //   o
        // x
        if (
            Cell.x >= 1
            && Cell.y <= (GAME_GRID_HEIGHT - 2)
            && IsCellAlive(Current, GetCellPositionAdj(i, -1, 1))
        ) Neighbours++;

        //
        //   o
        //   x
        if (
            Cell.y <= (GAME_GRID_HEIGHT - 2)
            && IsCellAlive(Current, GetCellPositionAdj(i, 0, 1))
        ) Neighbours++;

        //
        //   o
        //     x
        if (
            Cell.x <= (GAME_GRID_WIDTH - 2)
            && Cell.y <= (GAME_GRID_HEIGHT - 2)
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
            Cell.x <= (GAME_GRID_WIDTH - 2)
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
            Cell.x <= (GAME_GRID_WIDTH - 2)
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
    for (int i = 0; i < TOTAL_GRID_SIZE; i++)
    {
        const SDL_Point Cell = GetCellPosition(i);
        SetCellState(grid, Cell, DEAD);
    }
}



void DrawGosperGliderGun(CellState *grid, SDL_Point pos)
{
    // https://en.wikipedia.org/wiki/Gun_(cellular_automaton)

    SDL_Rect BoundRect = {
        .x = pos.x,
        .y = pos.y,
        .w = 38,
        .h = 11,
    };

    SetCellState(grid, (SDL_Point){ .x = 25 + pos.x, .y = 1 + pos.y }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 23 + pos.x, .y = 2 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 25 + pos.x, .y = 2 + pos.y }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 13 + pos.x, .y = 3 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 14 + pos.x, .y = 3 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 21 + pos.x, .y = 3 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 22 + pos.x, .y = 3 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 35 + pos.x, .y = 3 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 36 + pos.x, .y = 3 + pos.y }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 12 + pos.x, .y = 4 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 16 + pos.x, .y = 4 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 21 + pos.x, .y = 4 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 22 + pos.x, .y = 4 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 35 + pos.x, .y = 4 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 36 + pos.x, .y = 4 + pos.y }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 1 + pos.x, .y = 5 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 2 + pos.x, .y = 5 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 11 + pos.x, .y = 5 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 17 + pos.x, .y = 5 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 21 + pos.x, .y = 5 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 22 + pos.x, .y = 5 + pos.y }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 1 + pos.x, .y = 6 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 2 + pos.x, .y = 6 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 11 + pos.x, .y = 6 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 15 + pos.x, .y = 6 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 17 + pos.x, .y = 6 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 18 + pos.x, .y = 6 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 23 + pos.x, .y = 6 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 25 + pos.x, .y = 6 + pos.y }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 11 + pos.x, .y = 7 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 17 + pos.x, .y = 7 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 25 + pos.x, .y = 7 + pos.y }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 12 + pos.x, .y = 8 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 16 + pos.x, .y = 8 + pos.y }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 13 + pos.x, .y = 9 + pos.y }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 14 + pos.x, .y = 9 + pos.y }, ALIVE);
}
