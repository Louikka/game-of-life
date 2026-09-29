#include <SDL3/SDL_rect.h>

#include "game_grid.h"

#include "presets.h"



void Preset_GosperGliderGun(CellState *grid, SDL_Point pos)
{
    // https://en.wikipedia.org/wiki/Gun_(cellular_automaton)

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
