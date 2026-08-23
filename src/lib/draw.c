#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "game_grid.h"
#include "game_state.h"
#include "text.h"

#include "draw.h"


void DrawCurrentFrame(SDL_Renderer *r, GOL_GameState gs)
{
    const int TOTAL_GRID_SIZE = GameGridWidth * GameGridHeight;

    SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(r);

    SDL_SetRenderDrawColor(r, 255, 255, 255, SDL_ALPHA_OPAQUE);

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

            SDL_RenderFillRect(r, &rect);
        }
    }

    if (gs.IsGamePaused)
    {
        TTF_DrawRendererText(TextObj_Controls, 20, 20);
    }

    SDL_RenderPresent(r);
}
