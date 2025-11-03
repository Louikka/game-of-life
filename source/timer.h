#include "raylib.h"


typedef struct __timer
{
    float currentTime;
} Timer;


void StartTimer(Timer * timer, float time);

void UpdateTimer(Timer * timer);

bool IsTimerDone(Timer * timer);
