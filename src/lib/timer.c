#include <SDL3/SDL_timer.h>

#include "timer.h"


void UpdateTimer(GOL_GameTimer *t)
{
    Uint64 TicksNS = SDL_GetTicksNS();

    float Delta = (float)(TicksNS - t->Ticks) / SDL_NS_PER_SECOND;

    t->Ticks = TicksNS;
    t->Active += Delta;
}
