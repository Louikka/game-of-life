#include <stdlib.h>
#include <string.h>

#include "raylib.h"


#define GRID_WIDTH 900
#define GRID_HEIGHT 600

#define CELL_SIZE 10


typedef enum __cell_state
{
    DEAD,
    ALIVE,
} CellState;


int main(void)
{
    const int SCREEN_WIDTH = GRID_WIDTH;
    const int SCREEN_HEIGHT = GRID_HEIGHT;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conway's Game of Life");
    SetTargetFPS(6);


    const int __arr_max_row = GRID_HEIGHT / CELL_SIZE - 1;
    const int __arr_max_col = GRID_WIDTH / CELL_SIZE - 1;

    // [ rows ] [ columns ]
    CellState currentGeneration[GRID_HEIGHT / CELL_SIZE][GRID_WIDTH / CELL_SIZE] = { DEAD };
    CellState nextGeneration[GRID_HEIGHT / CELL_SIZE][GRID_WIDTH / CELL_SIZE] = { DEAD };

    //bool isGameRunning = true;
    //unsigned int noOfGenerations = 1;

    currentGeneration[5][6] = ALIVE;
    currentGeneration[6][7] = ALIVE;
    currentGeneration[7][5] = ALIVE;
    currentGeneration[7][6] = ALIVE;
    currentGeneration[7][7] = ALIVE;

    currentGeneration[20][20] = ALIVE;
    currentGeneration[20][21] = ALIVE;
    currentGeneration[20][22] = ALIVE;

    currentGeneration[30][45] = ALIVE;
    currentGeneration[30][46] = ALIVE;
    currentGeneration[30][47] = ALIVE;
    currentGeneration[30][48] = ALIVE;
    currentGeneration[30][49] = ALIVE;
    currentGeneration[30][50] = ALIVE;

    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(BLACK);

            for (int row = 0; row < GRID_HEIGHT / CELL_SIZE; row++)
            {
                for (int col = 0; col < GRID_WIDTH / CELL_SIZE; col++)
                {
                    // draw cell of current generation
                    if (currentGeneration[row][col] == ALIVE)
                    {
                        DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, WHITE);
                    }

                    // update cell of next generation
                    unsigned short int noOfNeighbours = 0;

                    if (row > 1             && col > 1             && currentGeneration[row - 1][col - 1] == ALIVE) noOfNeighbours++;
                    if (row > 1                                    && currentGeneration[row - 1][col]     == ALIVE) noOfNeighbours++;
                    if (row > 1             && col < __arr_max_col && currentGeneration[row - 1][col + 1] == ALIVE) noOfNeighbours++;
                    if (                       col > 1             && currentGeneration[row]    [col - 1] == ALIVE) noOfNeighbours++;
                    if (                       col < __arr_max_col && currentGeneration[row]    [col + 1] == ALIVE) noOfNeighbours++;
                    if (row < __arr_max_row && col > 1             && currentGeneration[row + 1][col - 1] == ALIVE) noOfNeighbours++;
                    if (row < __arr_max_row                        && currentGeneration[row + 1][col]     == ALIVE) noOfNeighbours++;
                    if (row < __arr_max_row && col < __arr_max_col && currentGeneration[row + 1][col + 1] == ALIVE) noOfNeighbours++;

                    if (currentGeneration[row][col] == DEAD && noOfNeighbours == 3)
                    {
                        nextGeneration[row][col] = ALIVE;
                    }
                    else if (currentGeneration[row][col] == ALIVE && (noOfNeighbours == 2 || noOfNeighbours == 3))
                    {
                        nextGeneration[row][col] = ALIVE;
                    }
                    else if (currentGeneration[row][col] == ALIVE && (noOfNeighbours < 2 || noOfNeighbours > 3))
                    {
                        nextGeneration[row][col] = DEAD;
                    }
                    else
                    {
                        nextGeneration[row][col] = DEAD;
                    }
                }
            }

        EndDrawing();


        // copy next gen to current
        for (int row = 0; row < GRID_HEIGHT / CELL_SIZE; row++)
        {
            for (int col = 0; col < GRID_WIDTH / CELL_SIZE; col++)
            {
                currentGeneration[row][col] = nextGeneration[row][col];
                nextGeneration[row][col] = DEAD;
            }
        }

        //noOfGenerations++;
    }


    CloseWindow();

    return 0;
}
