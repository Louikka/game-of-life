#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <SDL3/SDL.h>

/* https://github.com/rxi/vec */
#include "rxi/vec-0.2.1/src/vec.h"


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600


/** In seconds. */
#define GENERATION_LIVE_TIME 0.1f


#define CELL_SIZE 10



typedef struct {
    float active;
    Uint64 sinceLastFrame;
} Timer;



#define DEAD 0
#define ALIVE 1

typedef int CellState;



typedef SDL_Point CellCoordinates;
typedef vec_t(SDL_Point) vec_SDL_Point; // https://github.com/rxi/vec?tab=readme-ov-file#types

/** Translates cell's absolute coordinates to relative (to viewport position). */
SDL_Point GetCellRelCoordinates(SDL_Point viewport, SDL_Point cell)
{
    return (SDL_Point){
        .x = cell.x - viewport.x,
        .y = cell.y - viewport.y,
    };
}

/** Translates cell's relative coordinates to absolute. */
SDL_Point GetCellAbsCoordinates(SDL_Point viewport, SDL_Point cell)
{
    return (SDL_Point){
        .x = cell.x + viewport.x,
        .y = cell.y + viewport.y,
    };
}


/** Returns index of the element (`Cell`) in vector or -1 if no such cell present. */
int _GetCellIndex(vec_SDL_Point grid, SDL_Point cell)
{
    for (int i = 0; i < grid.length; i++)
    {
        if (grid.data[i].x == cell.x && grid.data[i].y == cell.y)
        {
            return i;
        }
    }

    return -1;
}

bool IsCellAlive(vec_SDL_Point grid, SDL_Point cell)
{
    for (int i = 0; i < grid.length; i++)
    {
        if (grid.data[i].x == cell.x && grid.data[i].y == cell.y)
        {
            return true;
        }
    }

    return false;
}


vec_SDL_Point* UpdateGrid(vec_SDL_Point *current)
{
    vec_SDL_Point next;
    vec_init(&next);

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

        /*
        if (noOfNeighbours < 2 || noOfNeighbours > 3)
        {
            // deadge
        }
        */
        if (noOfNeighbours == 2 || noOfNeighbours == 3)
        {
            int success = vec_push(&next, ((SDL_Point){ current->data[i].x, current->data[i].y, }));
            if (success == -1)
            {
                printf("Cannot push new element to a vector.\n");
            }
        }
    }

    /*
    // copy next generation grid to the current grid
    for (int i = 0; i < CELLS_IN_GRID_HEIGHT; i++)
    {
        for (int j = 0; j < CELLS_IN_GRID_WIDTH; j++)
        {
            SetGridCellState(current, i, j, IsCellAlive(next, i, j) ? ALIVE : DEAD);
            SetGridCellState(next, i, j, DEAD);
        }
    }
    */

    vec_deinit(&next);

    return current;
}

int main(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Something went wrong while initializing SDL window : %s.\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Conway's Game of Life", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer))
    {
        printf("Could not create window or renderer : %s.\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }


    bool isMainLoopActive = true;

    Timer timer = {
        .active = 0,
        .sinceLastFrame = SDL_GetTicksNS(),
    };

    SDL_Point viewportCoordinates = {
        .x = 0,
        .y = 0,
    };


    bool isGamePaused = true;

    // Game world grid initialisation.
    //
    // Game world is ***infinitely*** large. Starting position of the viewport
    // are in the top left corner.

    vec_SDL_Point grid_CurrentGen;
    vec_init(&grid_CurrentGen);

    vec_SDL_Point grid_NextGen;
    vec_init(&grid_NextGen);

    int totalGenerations = 0;


    // more accuracy???
    timer.sinceLastFrame = SDL_GetTicksNS();

    while (isMainLoopActive)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    isMainLoopActive = false;
                    break;
                }

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    if (event.button.down)
                    {
                        int mousePosX = (int)(event.button.x / CELL_SIZE);
                        int mousePosY = (int)(event.button.y / CELL_SIZE);

                        if (event.button.button == SDL_BUTTON_RIGHT)
                        {
                            printf("Right mouse click detected at (%d, %d).\n", (int)event.button.x, (int)event.button.y);
                        }
                        else if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            printf("Left mouse click detected at (%d, %d).\n", (int)event.button.x, (int)event.button.y);

                            SDL_Point __cellAbsPos = GetCellAbsCoordinates(
                                viewportCoordinates,
                                (SDL_Point){ .x = mousePosX, .y = mousePosY, }
                            );

                            if (IsCellAlive(grid_CurrentGen, __cellAbsPos))
                            {
                                int i = __GetCellIndex(grid_CurrentGen, __cellAbsPos);
                                vec_splice(&grid_CurrentGen, i, 1);
                            }
                            else
                            {
                                int success = vec_push(&grid_CurrentGen, (__cellAbsPos));
                                if (success == -1)
                                {
                                    printf("Cannot push new element to a vector.\n");
                                }
                            }
                        }
                    }
                    break;
                }

                case SDL_EVENT_KEY_DOWN:
                {
                    /* Space button press - run/pause simulation. */
                    if (event.key.key == SDLK_SPACE && !event.key.repeat)
                    {
                        if (isGamePaused)
                        {
                            isGamePaused = false;
                            timer.active = 0;
                        }
                        else
                        {
                            isGamePaused = true;
                        }
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

        for (int i = 0; i < grid_CurrentGen.length; i++)
        {
            SDL_FRect rect = {
                .x = grid_CurrentGen.data[i].x * CELL_SIZE,
                .y = grid_CurrentGen.data[i].y * CELL_SIZE,
                .w = CELL_SIZE,
                .h = CELL_SIZE,
            };
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);

        if (!isGamePaused && timer.active > GENERATION_LIVE_TIME)
        {
            //UpdateGrid(grid_CurrentGen, grid_NextGen);
            totalGenerations++;

            timer.active = 0;
        }


        SDL_Delay(50); // approx. 20fps
    }

    vec_deinit(&grid_CurrentGen);
    vec_deinit(&grid_NextGen);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
