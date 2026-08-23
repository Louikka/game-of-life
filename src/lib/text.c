#include <SDL3_ttf/SDL_ttf.h>

#include "text.h"


TTF_Font *Font_JetBrainsMonoRegular = NULL;

bool LoadFonts()
{
    Font_JetBrainsMonoRegular = TTF_OpenFont("JetBrainsMono-Regular.ttf", 16);
    if (Font_JetBrainsMonoRegular == NULL)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to open \"JetBrainsMono-Regular.ttf\" : %s\n",
            SDL_GetError()
        );

        return false;
    }

    return true;
}

void CloseFonts()
{
    TTF_CloseFont(Font_JetBrainsMonoRegular);
    Font_JetBrainsMonoRegular = NULL;
}


TTF_Text *TextObj_Controls = NULL;

bool CreateTextObjects(TTF_TextEngine *engine)
{
    TextObj_Controls = TTF_CreateText(
        engine,
        Font_JetBrainsMonoRegular,
        "Press Space to start/stop the simulation. Press c to clear game grid. "
        "Press q or Esc to exit.",
        0
    );
    if (TextObj_Controls == NULL)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to create text : controls : %s\n",
            SDL_GetError()
        );

        return false;
    }
    else
    {
        TTF_SetTextColor(TextObj_Controls, 255, 255, 255, 255);
    }

    return true;
}

void DestroyTextObjects()
{
    TTF_DestroyText(TextObj_Controls);
    TextObj_Controls = NULL;
}
