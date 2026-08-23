#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "lib/game_grid.h"
#include "lib/game_state.h"
#include "lib/text.h"
#include "lib/timer.h"
#include "lib/draw.h"
#include "lib/presets.h"


int main()
{
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);



    /* Initializing SDL and window and renderer ******************************/

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


    SDL_Window *Window = NULL;
    SDL_Renderer *Renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Conway's Game of Life", 0, 0,
                                     SDL_WINDOW_FULLSCREEN, &Window, &Renderer))
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Could not create window or renderer : %s\n",
            SDL_GetError()
        );

        return 0;
    }


    SDL_Rect WindowRect;

    if (!SDL_GetWindowSize(Window, &WindowRect.w, &WindowRect.h))
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_SYSTEM,
            "Failed to get window size : %s\n",
            SDL_GetError()
        );
    }
    else
    {
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "Window size: %dx%d.\n",
            WindowRect.w, WindowRect.h
        );

        CalculateGameGridDimensions(WindowRect);
    }



    /* Setting up fonts and text *********************************************/

    TTF_TextEngine *RendererTextEngine = TTF_CreateRendererTextEngine(Renderer);
    if (RendererTextEngine == NULL)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to create text engine : %s\n",
            SDL_GetError()
        );
    }
    else
    {
        LoadFonts();
        CreateTextObjects(RendererTextEngine);
    }



    /* Grid initialisation ***************************************************/

    const bool ok = InitGameGrid();
    if (!ok)
    {
        goto SkipMainLoop;
    }



    /* Main game loop ********************************************************/

    GOL_GameState GameState = {
        .IsGamePaused = true,
        .GenerationLiveTime = 0.1,
        .TotalGenerations = 0,
    };

    GOL_GameTimer Timer = {
        .Ticks = SDL_GetTicksNS(),
        .Active = 0,
    };

    while (true)
    {
        UpdateTimer(&Timer);

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
                    // Space button press — run/pause simulation.
                    if (event.key.key == SDLK_SPACE && !event.key.repeat)
                    {
                        if (GameState.IsGamePaused)
                        {
                            GameState.IsGamePaused = false;
                            Timer.Active = 0;
                        }
                        else
                        {
                            GameState.IsGamePaused = true;
                        }
                    }
                    // Escape/Q — quit
                    else if (event.key.key == SDLK_ESCAPE || event.key.key == SDLK_Q)
                    {
                        SDL_Event ev;
                        ev.type = SDL_EVENT_QUIT;
                        SDL_PushEvent(&ev);
                    }
                    // C — clear canvas
                    else if (event.key.key == SDLK_C)
                    {
                        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Clearing grid...");
                        ClearGrid(GameGrid_Current);
                    }
                    // + — increase game speed
                    else if (event.key.key == SDLK_PLUS || event.key.key == SDLK_KP_PLUS)
                    {
                        // TODO: implement speed controls
                    }
                    // - — decrease game speed
                    else if (event.key.key == SDLK_MINUS || event.key.key == SDLK_KP_MINUS)
                    {
                        // TODO: implement speed controls
                    }
                    // G — draw a glider
                    else if (event.key.key == SDLK_G)
                    {
                        Preset_GosperGliderGun(GameGrid_Current,
                                               (SDL_Point){ .x = 10, .y = 10 });
                    }

                    break;
                }
            }
        }

        DrawCurrentFrame(Renderer, GameState);

        if (!GameState.IsGamePaused && Timer.Active > GameState.GenerationLiveTime)
        {
            UpdateGrid(GameGrid_Current, GameGrid_Next);
            GameState.TotalGenerations++;
            Timer.Active = 0;
        }

        SDL_Delay(50);
    }

MainLoopEnd:
SkipMainLoop:


    DestroyTextObjects();
    CloseFonts();

    TTF_DestroyRendererTextEngine(RendererTextEngine);

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);

    return 0;
}
