// from this tutorial
// https://www.youtube.com/watch?v=vGlvTWUctTQ

#include "raylib.h"

#include "timer.h"


void StartTimer(Timer * timer, float time)
{
    timer->currentTime = time;
}

void UpdateTimer(Timer * timer)
{
    if (timer->currentTime > 0)
    {
        timer->currentTime -= GetFrameTime();
    }
}

bool IsTimerDone(Timer * timer)
{
    return timer->currentTime <= 0;
}
