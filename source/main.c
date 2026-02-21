#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <SDL3/SDL.h>

/* https://github.com/rxi/vec */
#include "rxi/vec-0.2.1/src/vec.h"
#include "vec.h"


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600


/** In seconds. */
#define GENERATION_LIVE_TIME 0.1f


#define CELL_SIZE 10

#define WORLD_SIZE_WIDTH (1000*CELL_SIZE)
#define WORLD_SIZE_HEIGHT (1000*CELL_SIZE)


#define CELLS_IN_GRID_WIDTH 80 // SCREEN_WIDTH / CELL_SIZE
#define CELLS_IN_GRID_HEIGHT 60 // SCREEN_HEIGHT / CELL_SIZE
#define TOTAL_CELLS_IN_GRID ((CELLS_IN_GRID_WIDTH)*(CELLS_IN_GRID_HEIGHT))

/** Alias for `CELLS_IN_GRID_HEIGHT`. */
#define ROWS_IN_GRID CELLS_IN_GRID_HEIGHT
/** Alias for `CELLS_IN_GRID_WIDTH`. */
#define COLS_PER_ROW CELLS_IN_GRID_WIDTH

#define GRID_MAX_ROW ((ROWS_IN_GRID)-1)
#define GRID_MAX_COL ((COLS_PER_ROW)-1)


typedef enum {
    DEAD,
    ALIVE,
} CellState;


/** Translates coordinates (SDL_Point) from viewport to window. */
SDL_Point TranslateViewportCoordinates(SDL_Point point)
{
    SDL_Point p = { .x = 0, .y = 0, };
    return p;
}


void SetGridCellState(CellState *grid, int row, int col, CellState state)
{
    CellState *cell = grid + (COLS_PER_ROW * row) + col;
    *cell = state;
}

bool IsCellAlive(CellState *grid, int row, int col)
{
    CellState cell = *(grid + (COLS_PER_ROW * row) + col);
    return cell == ALIVE;
}


void UpdateGrid(CellState *current, CellState *next)
{
    for (int row = 0; row < ROWS_IN_GRID; row++)
    {
        for (int col = 0; col < COLS_PER_ROW; col++)
        {
            unsigned char noOfNeighbours = 0;

            if (row > 0            && col > 0            && IsCellAlive(current, row - 1, col - 1)) noOfNeighbours++;
            if (row > 0                                  && IsCellAlive(current, row - 1, col)    ) noOfNeighbours++;
            if (row > 0            && col < GRID_MAX_COL && IsCellAlive(current, row - 1, col + 1)) noOfNeighbours++;
            if (                      col > 0            && IsCellAlive(current, row,     col - 1)) noOfNeighbours++;
            if (                      col < GRID_MAX_COL && IsCellAlive(current, row,     col + 1)) noOfNeighbours++;
            if (row < GRID_MAX_ROW && col > 0            && IsCellAlive(current, row + 1, col - 1)) noOfNeighbours++;
            if (row < GRID_MAX_ROW                       && IsCellAlive(current, row + 1, col)    ) noOfNeighbours++;
            if (row < GRID_MAX_ROW && col < GRID_MAX_COL && IsCellAlive(current, row + 1, col + 1)) noOfNeighbours++;

            if (
                (!IsCellAlive(current, row, col) && noOfNeighbours == 3) ||
                (IsCellAlive(current, row, col) && (noOfNeighbours == 2 || noOfNeighbours == 3))
            )
            {
                SetGridCellState(next, row, col, ALIVE);
            }
            else
            {
                SetGridCellState(next, row, col, DEAD);
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
}


int main(void)
{
    CellState gridCurrentGen[TOTAL_CELLS_IN_GRID] = { DEAD };
    CellState gridNextGen[TOTAL_CELLS_IN_GRID] = { DEAD };

    unsigned int noOfGenerations = 0;


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


    bool isAppRunning = true;

    float timer = 0;
    Uint64 lastFrameTime = SDL_GetTicksNS();

    printf("SDL_GetTicksNS() before main loop : %" PRIu64 ".\n", lastFrameTime);

    bool isGamePaused = true;

    while (isAppRunning)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    isAppRunning = false;
                    break;
                }

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    if (event.button.down)
                    {
                        int rowUnderMouse = (int)(event.button.y / CELL_SIZE);
                        int colUnderMouse = (int)(event.button.x / CELL_SIZE);

                        printf(
                            "Mouse click detected at (%f, %f). Changing cell at (%i, %i).\n",
                            event.button.x, event.button.y,
                            rowUnderMouse, colUnderMouse
                        );

                        SetGridCellState(
                            gridCurrentGen,
                            rowUnderMouse,
                            colUnderMouse,
                            IsCellAlive(gridCurrentGen, rowUnderMouse, colUnderMouse) ? DEAD : ALIVE
                        );
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
                            timer = 0;
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
        float delta = (float)(currentFrameTime - lastFrameTime) / (float)SDL_NS_PER_SECOND;
        lastFrameTime = currentFrameTime;

        timer += delta;


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

        for (int row = 0; row < ROWS_IN_GRID; row++)
        {
            for (int col = 0; col < COLS_PER_ROW; col++)
            {
                if (IsCellAlive(gridCurrentGen, row, col))
                {
                    SDL_Rect rect = { col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);

        if (!isGamePaused && timer > GENERATION_LIVE_TIME)
        {
            UpdateGrid(gridCurrentGen, gridNextGen);
            noOfGenerations++;

            printf("Generation %u, time elapsed since last : %fs.\n", noOfGenerations, timer);

            timer = 0;
        }


        SDL_Delay(50); // approx. 20fps
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
