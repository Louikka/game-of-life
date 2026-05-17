#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


#define CELL_SIZE 10

#define GENERATION_LIVE_TIME 0.1f // In seconds.


typedef char CellState;

// two possible states of cell
#define DEAD 0
#define ALIVE 1


static int GAME_GRID_WIDTH = 0;
static int GAME_GRID_HEIGHT = 0;

static int TOTAL_GRID_SIZE = 0;


/**
 * @param `i` index in the array.
 */
static SDL_Point GetCellPosition(int i)
{
    return (SDL_Point){
        .x = (i % GAME_GRID_WIDTH),
        .y = (int)(i / GAME_GRID_WIDTH),
    };
}
/** Returns `SDL_Point` struct with positions adjusted with corresponded parameters. */
static SDL_Point GetCellPositionAdj(int i, int x, int y)
{
    return (SDL_Point){
        .x = (i % GAME_GRID_WIDTH) + x,
        .y = (int)(i / GAME_GRID_WIDTH) + y,
    };
}


static CellState GetCellState(CellState *grid, SDL_Point pos)
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

static void SetCellState(CellState *grid, SDL_Point pos, CellState state)
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

static bool IsCellAlive(CellState *grid, SDL_Point pos)
{
    return GetCellState(grid, pos) == ALIVE;
}

static void ToggleCellState(CellState *grid, SDL_Point pos)
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


static void UpdateGrid(CellState *current, CellState *next)
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

static void ClearGrid(CellState *grid)
{
    for (int i = 0; i < TOTAL_GRID_SIZE; i++)
    {
        const SDL_Point cell = GetCellPosition(i);
        SetCellState(grid, cell, DEAD);
    }
}


static void DrawGosperGliderGun(CellState *grid, SDL_Point pos)
{
    // https://en.wikipedia.org/wiki/Gun_(cellular_automaton)

    SDL_Rect boundRect = {
        .x = pos.x,
        .y = pos.y,
        .w = 38,
        .h = 11,
    };

    SetCellState(grid, (SDL_Point){ .x = 25 + pos.x, .y = 1 + pos.y, }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 23 + pos.x, .y = 2 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 25 + pos.x, .y = 2 + pos.y, }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 13 + pos.x, .y = 3 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 14 + pos.x, .y = 3 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 21 + pos.x, .y = 3 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 22 + pos.x, .y = 3 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 35 + pos.x, .y = 3 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 36 + pos.x, .y = 3 + pos.y, }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 12 + pos.x, .y = 4 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 16 + pos.x, .y = 4 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 21 + pos.x, .y = 4 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 22 + pos.x, .y = 4 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 35 + pos.x, .y = 4 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 36 + pos.x, .y = 4 + pos.y, }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 1 + pos.x, .y = 5 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 2 + pos.x, .y = 5 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 11 + pos.x, .y = 5 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 17 + pos.x, .y = 5 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 21 + pos.x, .y = 5 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 22 + pos.x, .y = 5 + pos.y, }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 1 + pos.x, .y = 6 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 2 + pos.x, .y = 6 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 11 + pos.x, .y = 6 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 15 + pos.x, .y = 6 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 17 + pos.x, .y = 6 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 18 + pos.x, .y = 6 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 23 + pos.x, .y = 6 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 25 + pos.x, .y = 6 + pos.y, }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 11 + pos.x, .y = 7 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 17 + pos.x, .y = 7 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 25 + pos.x, .y = 7 + pos.y, }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 12 + pos.x, .y = 8 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 16 + pos.x, .y = 8 + pos.y, }, ALIVE);

    SetCellState(grid, (SDL_Point){ .x = 13 + pos.x, .y = 9 + pos.y, }, ALIVE);
    SetCellState(grid, (SDL_Point){ .x = 14 + pos.x, .y = 9 + pos.y, }, ALIVE);
}


typedef struct {
    float active;
    Uint64 sinceLastFrame;
} Timer;


static SDL_Surface *CreateTextSurface(
    TTF_Font *font,
    const char *text,
    SDL_Color color,
    SDL_Rect *text_rect
)
{
    text_rect->x = 0;
    text_rect->y = 0;

    TTF_GetStringSize(
        font,
        text,
        0,
        &text_rect->w, &text_rect->h
    );

    return TTF_RenderText_Blended(
        font,
        text,
        0,
        color
    );
}





int main()
{
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);



    /* Initializing SDL library and creating window **************************/

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Something went wrong while initializing SDL library : %s\n",
            SDL_GetError()
        );
        return 0;
    }
    else
    {
        atexit(SDL_Quit);
    }

    if (!TTF_Init())
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Something went wrong while initializing SDL_ttf : %s\n",
            SDL_GetError()
        );
        return 0;
    }
    else
    {
        atexit(TTF_Quit);
    }


    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Conway's Game of Life", 0, 0, SDL_WINDOW_FULLSCREEN, &window, &renderer))
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Could not create window or renderer : %s\n",
            SDL_GetError()
        );
        return 0;
    }


    SDL_Rect window_rect;

    if (!SDL_GetWindowSize(window, &window_rect.w, &window_rect.h))
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Failed to get window size : %s\n",
            SDL_GetError()
        );
        goto DestroySDLRenderers;
    }
    else
    {
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "Window size: %dx%d.\n",
            window_rect.w, window_rect.h
        );
    }



    /* Setting up fonts and text *********************************************/

    TTF_Font *font_JetBrainsMono_Regular = TTF_OpenFont("JetBrainsMono-Regular.ttf", 16);
    if (font_JetBrainsMono_Regular == NULL)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed open a font : %s\n",
            SDL_GetError()
        );
        goto DestroySDLRenderers;
    }


    // controls
    SDL_Rect text_Controls_rect;
    SDL_Surface *text_Controls_surface = CreateTextSurface(
        font_JetBrainsMono_Regular,
        "Press Space to start/stop the simulation. Press c to clear game grid. Press q or Esc to exit.",
        (SDL_Color){255, 255, 255},
        &text_Controls_rect
    );
    SDL_Texture *text_Controls_texture = SDL_CreateTextureFromSurface(renderer, text_Controls_surface);



    /* Game world initialisation *********************************************/

    GAME_GRID_WIDTH = window_rect.w / CELL_SIZE;
    GAME_GRID_HEIGHT = window_rect.h / CELL_SIZE;

    if (GAME_GRID_WIDTH == 0 || GAME_GRID_HEIGHT == 0)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Failed to figure out the game grid size.\n"
        );
        goto CloseTTFFonts;
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
        goto CloseTTFFonts;
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
                    else if (event.key.key == SDLK_ESCAPE || event.key.key == SDLK_Q)
                    {
                        SDL_Event ev;
                        ev.type = SDL_EVENT_QUIT;
                        SDL_PushEvent(&ev);
                    }
                    else if (event.key.key == SDLK_C)
                    {
                        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Clearing grid...");
                        ClearGrid(GameGrid_Current);
                    }
                    else if (event.key.key == SDLK_G)
                    {
                        DrawGosperGliderGun(GameGrid_Current, (SDL_Point){ .x = 10, .y = 10, });
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

        if (IsGamePaused)
        {
            SDL_FRect _a;
            SDL_RectToFRect(&text_Controls_rect, &_a);

            _a.x = 20;
            _a.y = 20;

            SDL_RenderTexture(renderer, text_Controls_texture, NULL, &_a);
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

    SDL_DestroySurface(text_Controls_surface);
    SDL_DestroyTexture(text_Controls_texture);

CloseTTFFonts:

    TTF_CloseFont(font_JetBrainsMono_Regular);

DestroySDLRenderers:

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
