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


CellState GetCellState(CellState *grid, SDL_Point pos)
{
    return grid[pos.y * GAME_GRID_WIDTH + pos.x];
}

void SetCellState(CellState *grid, SDL_Point pos, CellState state)
{
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

/*
void UpdateGrid(CellState *current, CellState *next)
{
    for (int i = 0; i < current->length; i++)
    {
        unsigned int noOfNeighbours = 0;

        if (IsCellAlive(*current, (SDL_Point){ current->data[i].x - 1, current->data[i].y + 1, })) noOfNeighbours++;
        if (IsCellAlive(*current, (SDL_Point){ current->data[i].x,     current->data[i].y + 1, })) noOfNeighbours++;
        if (IsCellAlive(*current, (SDL_Point){ current->data[i].x + 1, current->data[i].y + 1, })) noOfNeighbours++;
        if (IsCellAlive(*current, (SDL_Point){ current->data[i].x - 1, current->data[i].y,     })) noOfNeighbours++;
        if (IsCellAlive(*current, (SDL_Point){ current->data[i].x + 1, current->data[i].y,     })) noOfNeighbours++;
        if (IsCellAlive(*current, (SDL_Point){ current->data[i].x - 1, current->data[i].y - 1, })) noOfNeighbours++;
        if (IsCellAlive(*current, (SDL_Point){ current->data[i].x,     current->data[i].y - 1, })) noOfNeighbours++;
        if (IsCellAlive(*current, (SDL_Point){ current->data[i].x + 1, current->data[i].y - 1, })) noOfNeighbours++;

        if (noOfNeighbours < 2 || noOfNeighbours > 3)
        {
            // deadge
        }
        if (noOfNeighbours == 2 || noOfNeighbours == 3)
        {
            int success = vec_push(&next, ((SDL_Point){ current->data[i].x, current->data[i].y, }));
            if (success == -1)
            {
                printf("Cannot push new element to a vector.\n");
            }
        }
    }

    // copy next generation grid to the current grid
    for (int i = 0; i < CELLS_IN_GRID_HEIGHT; i++)
    {
        for (int j = 0; j < CELLS_IN_GRID_WIDTH; j++)
        {
            SetGridCellState(current, i, j, IsCellAlive(next, i, j) ? ALIVE : DEAD);
            SetGridCellState(next, i, j, DEAD);
        }
    }

    vec_deinit(&next);

    return current;
}
*/



typedef struct {
    float active;
    Uint64 sinceLastFrame;
} Timer;



int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Something went wrong while initializing SDL library : %s.\n", SDL_GetError());
        return 0;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Conway's Game of Life", 0, 0, SDL_WINDOW_FULLSCREEN, &window, &renderer))
    {
        printf("Could not create window or renderer : %s.\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }


    int window_w, window_h;

    if (!SDL_GetWindowSize(window, &window_w, &window_h))
    {
        printf("Failed to get window size : %s.\n", SDL_GetError());
        goto ProgramEnd;
    }
    else
    {
        printf("Window size : %dx%d\n", window_w, window_h);
    }


    // Game world initialisation.

    GAME_GRID_WIDTH = window_w / CELL_SIZE;
    GAME_GRID_HEIGHT = window_h / CELL_SIZE;

    if (GAME_GRID_WIDTH == 0 || GAME_GRID_HEIGHT == 0)
    {
        printf("Failed to figure out the game grid size.\n");
        goto ProgramEnd;
    }

    TOTAL_GRID_SIZE = GAME_GRID_WIDTH * GAME_GRID_HEIGHT;

    printf(
        "Size of the game world : %dx%d. Total amount of cells in grid : %d\n",
        GAME_GRID_WIDTH, GAME_GRID_HEIGHT,
        TOTAL_GRID_SIZE
    );

    CellState *GameGrid_Current = (CellState*)calloc(TOTAL_GRID_SIZE, sizeof(CellState));
    CellState *GameGrid_Next = (CellState*)calloc(TOTAL_GRID_SIZE, sizeof(CellState));

    if (GameGrid_Current == NULL || GameGrid_Next == NULL)
    {
        printf("Failed to allocate memory for game grids.\n");
        goto ProgramEnd;
    }


    // Main game loop.

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
                            printf("Left mouse click detected at (%d, %d).\n", (int)event.button.x, (int)event.button.y);

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
            //UpdateGrid(grid_CurrentGen, grid_NextGen);
            TotalGenerations++;

            timer.active = 0;
        }

        SDL_Delay(50);
    }


MainLoopEnd:

    free(GameGrid_Current);
    free(GameGrid_Next);

ProgramEnd:

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
