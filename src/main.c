#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "grid.h"



typedef struct
{
    float active;
    Uint64 sinceLastFrame;
} Timer;


static SDL_Surface *CreateTextSurface(TTF_Font *font, const char *text,
                                      SDL_Color color, SDL_Rect *text_rect)
{
    text_rect->x = 0;
    text_rect->y = 0;

    TTF_GetStringSize(font, text, 0, &text_rect->w, &text_rect->h);

    return TTF_RenderText_Blended(font, text, 0, color);
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

    if (!SDL_CreateWindowAndRenderer("Conway's Game of Life", 0, 0,
                                     SDL_WINDOW_FULLSCREEN, &window, &renderer))
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
    char *text_Controls =
        "Press Space to start/stop the simulation. Press c to clear game grid. "
        "Press q or Esc to exit.";

    SDL_Rect text_Controls_rect;

    SDL_Surface *text_Controls_surface = CreateTextSurface(
        font_JetBrainsMono_Regular,
        text_Controls,
        (SDL_Color){255, 255, 255},
        &text_Controls_rect
    );

    SDL_Texture *text_Controls_texture =
        SDL_CreateTextureFromSurface(renderer, text_Controls_surface);



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

    CellState *GameGrid_Current = (CellState *)calloc(TOTAL_GRID_SIZE, sizeof(CellState));
    CellState *GameGrid_Next = (CellState *)calloc(TOTAL_GRID_SIZE, sizeof(CellState));

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
                        DrawGosperGliderGun(GameGrid_Current,
                                            (SDL_Point){ .x = 10, .y = 10, });
                    }

                    break;
                }
            }
        }


        Uint64 CurrentFrameTime = SDL_GetTicksNS();
        float delta = (float)(CurrentFrameTime - timer.sinceLastFrame) / (float)SDL_NS_PER_SECOND;
        timer.sinceLastFrame = CurrentFrameTime;
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
