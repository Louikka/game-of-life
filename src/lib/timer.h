#ifndef GAME_OF_LIFE_TIMER_H
#define GAME_OF_LIFE_TIMER_H



// in seconds
#define GAME_MIN_GENERATION_TIME 0.01f
#define GAME_MAX_GENERATION_TIME 1f


typedef struct
{
    /** Value of the `SDL_GetTicksNS()` assigned on the last timer update. */
    Uint64 Ticks;
    /** Current active time (in seconds). */
    float Active;
} GOL_GameTimer;


void UpdateTimer(GOL_GameTimer *t);



#endif
