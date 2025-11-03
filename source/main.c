#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "timer.h"


#define CELL_SIZE 10

#define CELLS_IN_GRID_WIDHT 90
#define CELLS_IN_GRID_HEIGHT 60

#define ROWS_IN_GRID CELLS_IN_GRID_WIDHT
#define COLS_IN_GRID CELLS_IN_GRID_HEIGHT


/**
 * `DEAD = 0`
 * `ALIVE = 1`
 */
typedef enum __cell_state
{
    DEAD,
    ALIVE,
} CellState;


void UpdateNextGrid(CellState curr[COLS_IN_GRID][ROWS_IN_GRID], CellState next[COLS_IN_GRID][ROWS_IN_GRID]);


int main(void)
{
    const unsigned int SCREEN_WIDTH = CELLS_IN_GRID_WIDHT * CELL_SIZE;
    const unsigned int SCREEN_HEIGHT = CELLS_IN_GRID_HEIGHT * CELL_SIZE;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conway's Game of Life");
    SetTargetFPS(30);



    /**
     * Current generation grid (`[ rows ]` `[ columns ]`).
     */
    CellState currGenGrid[COLS_IN_GRID][ROWS_IN_GRID] = { DEAD };
    /**
     * Next generation grid (`[ rows ]` `[ columns ]`).
     */
    CellState nextGenGrid[COLS_IN_GRID][ROWS_IN_GRID] = { DEAD };


    bool isGameRunning = false;

    const float GAME_SPEED = 0.1f; // seconds beetween each generation
    Timer GameTimer = { 0 };

    unsigned int noOfGenerations = 1;

    // for test purposes
    currGenGrid[5][6] = ALIVE;
    currGenGrid[6][7] = ALIVE;
    currGenGrid[7][5] = ALIVE;
    currGenGrid[7][6] = ALIVE;
    currGenGrid[7][7] = ALIVE;

    currGenGrid[20][20] = ALIVE;
    currGenGrid[20][21] = ALIVE;
    currGenGrid[20][22] = ALIVE;

    currGenGrid[30][45] = ALIVE;
    currGenGrid[30][46] = ALIVE;
    currGenGrid[30][47] = ALIVE;
    currGenGrid[30][48] = ALIVE;
    currGenGrid[30][49] = ALIVE;
    currGenGrid[30][50] = ALIVE;


    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            if (isGameRunning)
            {
                isGameRunning = false;
            }
            else
            {
                isGameRunning = true;
                StartTimer(&GameTimer, GAME_SPEED);
            }
        }

        UpdateTimer(&GameTimer);

        BeginDrawing();

            ClearBackground(BLACK);

            DrawText(TextFormat("Generation %u", noOfGenerations), 10, 10, 20, RAYWHITE);
            DrawText("Press SPACE to play/pause game", 10, 35, 10, RAYWHITE);

            // draw cells of current grid
            for (int row = 0; row < ROWS_IN_GRID; row++)
            {
                for (int col = 0; col < COLS_IN_GRID; col++)
                {
                    if (currGenGrid[row][col] == ALIVE)
                    {
                        DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, WHITE);
                    }
                }
            }


            if (isGameRunning && IsTimerDone(&GameTimer))
            {
                UpdateNextGrid(currGenGrid, nextGenGrid);

                noOfGenerations++;

                StartTimer(&GameTimer, GAME_SPEED);
            }

        EndDrawing();
    }


    CloseWindow();

    return 0;
}



void UpdateNextGrid(CellState curr[COLS_IN_GRID][ROWS_IN_GRID], CellState next[COLS_IN_GRID][ROWS_IN_GRID])
{
    const unsigned int __gridMaxRow = ROWS_IN_GRID - 1;
    const unsigned int __gridMaxCol = COLS_IN_GRID - 1;

    for (int row = 0; row < ROWS_IN_GRID; row++)
    {
        for (int col = 0; col < COLS_IN_GRID; col++)
        {
            // update cell of next grid
            unsigned char noOfNeighbours = 0;

            if (row > 1            && col > 1            && curr[row - 1][col - 1] == ALIVE) noOfNeighbours++;
            if (row > 1                                  && curr[row - 1][col]     == ALIVE) noOfNeighbours++;
            if (row > 1            && col < __gridMaxCol && curr[row - 1][col + 1] == ALIVE) noOfNeighbours++;
            if (                      col > 1            && curr[row]    [col - 1] == ALIVE) noOfNeighbours++;
            if (                      col < __gridMaxCol && curr[row]    [col + 1] == ALIVE) noOfNeighbours++;
            if (row < __gridMaxRow && col > 1            && curr[row + 1][col - 1] == ALIVE) noOfNeighbours++;
            if (row < __gridMaxRow                       && curr[row + 1][col]     == ALIVE) noOfNeighbours++;
            if (row < __gridMaxRow && col < __gridMaxCol && curr[row + 1][col + 1] == ALIVE) noOfNeighbours++;

            if (curr[row][col] == DEAD && noOfNeighbours == 3)
            {
                next[row][col] = ALIVE;
            }
            else if (curr[row][col] == ALIVE && (noOfNeighbours == 2 || noOfNeighbours == 3))
            {
                next[row][col] = ALIVE;
            }
            else if (curr[row][col] == ALIVE && (noOfNeighbours < 2 || noOfNeighbours > 3))
            {
                next[row][col] = DEAD;
            }
            else
            {
                next[row][col] = DEAD;
            }
        }
    }

    // copy next generation grid to the current grid
    for (int i = 0; i < CELLS_IN_GRID_HEIGHT; i++)
    {
        for (int j = 0; j < CELLS_IN_GRID_WIDHT; j++)
        {
            curr[i][j] = next[i][j];
            next[i][j] = DEAD;
        }
    }
}
