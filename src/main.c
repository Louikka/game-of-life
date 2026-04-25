#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <SDL3/SDL.h>


#define CELL_SIZE 10

#define GENERATION_LIVE_TIME 0.1f // In seconds.

#define DEAD 0
#define ALIVE 1

typedef char CellState;


int GAME_GRID_WIDTH = 0;
int GAME_GRID_HEIGHT = 0;

int TOTAL_GRID_SIZE = 0;


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

void SetCellState(CellState *grid, SDL_Point pos, CellState state)
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

    grid[pos.y * GAME_GRID_WIDTH + pos.x] = state;
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


void UpdateGrid(CellState *current, CellState *next)
{
    for (int i = 0; i < TOTAL_GRID_SIZE; i++)
    {
        const SDL_Point cell = GetCellPosition(i);

        unsigned int neighbours = 0;

        //
        //   o
        // x
        if (
            cell.x >= 1 &&
            cell.y <= (GAME_GRID_HEIGHT - 2) &&
            IsCellAlive(current, GetCellPositionAdj(i, -1, 1))
        ) neighbours++;

        //
        //   o
        //   x
        if (
            cell.y <= (GAME_GRID_HEIGHT - 2) &&
            IsCellAlive(current, GetCellPositionAdj(i, 0, 1))
        ) neighbours++;

        //
        //   o
        //     x
        if (
            cell.x <= (GAME_GRID_WIDTH - 2) &&
            cell.y <= (GAME_GRID_HEIGHT - 2) &&
            IsCellAlive(current, GetCellPositionAdj(i, 1, 1))
        ) neighbours++;

        //
        // x o
        //
        if (
            cell.x >= 1 &&
            IsCellAlive(current, GetCellPositionAdj(i, -1, 0))
        ) neighbours++;

        //
        //   o x
        //
        if (
            cell.x <= (GAME_GRID_WIDTH - 2) &&
            IsCellAlive(current, GetCellPositionAdj(i, 1, 0))
        ) neighbours++;

        // x
        //   o
        //
        if (
            cell.x >= 1 &&
            cell.y >= 1 &&
            IsCellAlive(current, GetCellPositionAdj(i, -1, -1))
        ) neighbours++;

        //   x
        //   o
        //
        if (
            cell.y >= 1 &&
            IsCellAlive(current, GetCellPositionAdj(i,  0, -1))
        ) neighbours++;

        //     x
        //   o
        //
        if (
            cell.x <= (GAME_GRID_WIDTH - 2) &&
            cell.y >= 1 &&
            IsCellAlive(current, GetCellPositionAdj(i,  1, -1))
        ) neighbours++;

        /*
        if (IsCellAlive(current, GetCellPosition(i)) && neighbours < 2)
        {
            // deadge
        }
        else if (IsCellAlive(current, GetCellPosition(i)) && (neighbours == 2 || neighbours == 3))
        {
            // alivge
        }
        else if (IsCellAlive(current, GetCellPosition(i)) && neighbours > 3)
        {
            // deadge
        }
        else if (!IsCellAlive(current, GetCellPosition(i)) && neighbours == 3)
        {
            // alivge
        }
        */

        if (
            (!IsCellAlive(current, cell) && neighbours == 3) ||
            (IsCellAlive(current, cell) && (neighbours == 2 || neighbours == 3))
        )
        {
            SetCellState(next, cell, ALIVE);
        }
        else
        {
            SetCellState(next, cell, DEAD);
        }
    }

    // copy next generation grid to the current grid
    for (int i = 0; i < TOTAL_GRID_SIZE; i++)
    {
        const SDL_Point cell = GetCellPosition(i);

        SetCellState(
            current,
            cell,
            GetCellState(next, cell)
        );
        SetCellState(next, cell, DEAD);
    }
}



typedef struct {
    float active;
    Uint64 sinceLastFrame;
} Timer;



int main()
{
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);


    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Something went wrong while initializing SDL library : %s.\n",
            SDL_GetError()
        );
        return 0;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Conway's Game of Life", 0, 0, SDL_WINDOW_FULLSCREEN, &window, &renderer))
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Could not create window or renderer : %s.\n",
            SDL_GetError()
        );
        goto QuitSDL;
    }


    int window_w, window_h;

    if (!SDL_GetWindowSize(window, &window_w, &window_h))
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Failed to get window size : %s.\n",
            SDL_GetError()
        );
        goto DestroySDLRenderers;
    }
    else
    {
        SDL_Log("Window size : %dx%d\n", window_w, window_h);
    }


    // Game world initialisation.

    GAME_GRID_WIDTH = window_w / CELL_SIZE;
    GAME_GRID_HEIGHT = window_h / CELL_SIZE;

    if (GAME_GRID_WIDTH == 0 || GAME_GRID_HEIGHT == 0)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Failed to figure out the game grid size.\n"
        );
        goto DestroySDLRenderers;
    }

    TOTAL_GRID_SIZE = GAME_GRID_WIDTH * GAME_GRID_HEIGHT;

    SDL_Log(
        "Size of the game world : %dx%d. Total amount of cells in grid : %d\n",
        GAME_GRID_WIDTH, GAME_GRID_HEIGHT,
        TOTAL_GRID_SIZE
    );

    CellState *GameGrid_Current = (CellState*)calloc(TOTAL_GRID_SIZE, sizeof(CellState));
    CellState *GameGrid_Next = (CellState*)calloc(TOTAL_GRID_SIZE, sizeof(CellState));

    if (GameGrid_Current == NULL || GameGrid_Next == NULL)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Failed to allocate memory for game grids.\n"
        );
        goto DestroySDLRenderers;
    }



    /* Main game loop ********************************************************/

    bool IsGamePaused = true;
    unsigned int TotalGenerations = 0;

    Timer timer = {
        .active = 0,
        .sinceLastFrame = SDL_GetTicksNS(),
    };

    while (true)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    goto MainLoopEnd;
                }

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    if (event.button.down)
                    {
                        SDL_Point pos = {
                            .x = (int)(event.button.x / CELL_SIZE),
                            .y = (int)(event.button.y / CELL_SIZE),
                        };

                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            SDL_LogDebug(
                                SDL_LOG_CATEGORY_APPLICATION,
                                "Left mouse click detected at (%d, %d).\n",
                                (int)event.button.x,
                                (int)event.button.y
                            );

                            ToggleCellState(GameGrid_Current, pos);
                        }
                    }
                    break;
                }

                case SDL_EVENT_KEY_DOWN:
                {
                    // Space button press - run/pause simulation.
                    if (event.key.key == SDLK_SPACE && !event.key.repeat)
                    {
                        if (IsGamePaused)
                        {
                            IsGamePaused = false;
                            timer.active = 0;
                        }
                        else
                        {
                            IsGamePaused = true;
                        }
                    }

                    if (event.key.key == SDLK_ESCAPE)
                    {
                        SDL_Event ev;
                        ev.type = SDL_EVENT_QUIT;
                        SDL_PushEvent(&ev);
                        break;
                    }

                    break;
                }
            }
        }


        Uint64 currentFrameTime = SDL_GetTicksNS();
        float delta = (float)(currentFrameTime - timer.sinceLastFrame) / (float)SDL_NS_PER_SECOND;
        timer.sinceLastFrame = currentFrameTime;
        timer.active += delta;


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

        for (int i = 0; i < TOTAL_GRID_SIZE; i++)
        {
            SDL_Point pos = GetCellPosition(i);

            if (IsCellAlive(GameGrid_Current, pos))
            {
                SDL_FRect rect = {
                    .x = pos.x * CELL_SIZE,
                    .y = pos.y * CELL_SIZE,
                    .w = CELL_SIZE,
                    .h = CELL_SIZE,
                };

                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);


        if (!IsGamePaused && timer.active > GENERATION_LIVE_TIME)
        {
            UpdateGrid(GameGrid_Current, GameGrid_Next);
            TotalGenerations++;

            timer.active = 0;
        }

        SDL_Delay(50);
    }

MainLoopEnd:


    free(GameGrid_Current);
    free(GameGrid_Next);

DestroySDLRenderers:

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

QuitSDL:

    SDL_Quit();

    return 0;
}
