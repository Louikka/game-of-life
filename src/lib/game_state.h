#ifndef GAME_OF_LIFE_GAME_STATE_H
#define GAME_OF_LIFE_GAME_STATE_H



typedef struct
{
    bool IsGamePaused;
    /** Basically, speed of the game (in seconds). */
    float GenerationLiveTime;
    unsigned int TotalGenerations;
} GOL_GameState;



#endif
