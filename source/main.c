#include <stdlib.h>
#include <string.h>

#include "raylib.h"


#define CELL_SIZE 10

#define CELLS_IN_GRID_WIDHT 90
#define CELLS_IN_GRID_HEIGHT 60


/**
 * `DEAD = 0`
 * `ALIVE = 1`
 */
typedef enum __cell_state
{
    DEAD,
    ALIVE,
} CellState;


int main(void)
{
    const unsigned int SCREEN_WIDTH = CELLS_IN_GRID_WIDHT * CELL_SIZE;
    const unsigned int SCREEN_HEIGHT = CELLS_IN_GRID_HEIGHT * CELL_SIZE;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conway's Game of Life");
    SetTargetFPS(10);



    /**
     * Current generation grid (`[ rows ]` `[ columns ]`).
     */
    CellState currGenGrid[CELLS_IN_GRID_HEIGHT][CELLS_IN_GRID_WIDHT] = { DEAD };
    /**
     * Next generation grid (`[ rows ]` `[ columns ]`).
     */
    CellState nextGenGrid[CELLS_IN_GRID_HEIGHT][CELLS_IN_GRID_WIDHT] = { DEAD };

    const int __grid_max_row = CELLS_IN_GRID_HEIGHT - 1;
    const int __grid_max_col = CELLS_IN_GRID_WIDHT - 1;


    bool isGameRunning = false;
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
        if (IsKeyPressed(KEY_SPACE)) isGameRunning = !isGameRunning;

        BeginDrawing();

            ClearBackground(BLACK);

            DrawText(TextFormat("Generation %u", noOfGenerations), 10, 10, 20, RAYWHITE);
            DrawText("Press SPACE to play/pause game", 10, 35, 10, RAYWHITE);


            for (int row = 0; row < CELLS_IN_GRID_HEIGHT; row++)
            {
                for (int col = 0; col < CELLS_IN_GRID_WIDHT; col++)
                {
                    // draw cell of current grid
                    if (currGenGrid[row][col] == ALIVE)
                    {
                        DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, WHITE);
                    }
                }
            }

        EndDrawing();


        if (isGameRunning)
        {
            // update cell of next grid
            for (int row = 0; row < CELLS_IN_GRID_HEIGHT; row++)
            {
                for (int col = 0; col < CELLS_IN_GRID_WIDHT; col++)
                {
                    unsigned char noOfNeighbours = 0;

                    if (row > 1              && col > 1              && currGenGrid[row - 1][col - 1] == ALIVE) noOfNeighbours++;
                    if (row > 1                                      && currGenGrid[row - 1][col]     == ALIVE) noOfNeighbours++;
                    if (row > 1              && col < __grid_max_col && currGenGrid[row - 1][col + 1] == ALIVE) noOfNeighbours++;
                    if (                        col > 1              && currGenGrid[row]    [col - 1] == ALIVE) noOfNeighbours++;
                    if (                        col < __grid_max_col && currGenGrid[row]    [col + 1] == ALIVE) noOfNeighbours++;
                    if (row < __grid_max_row && col > 1              && currGenGrid[row + 1][col - 1] == ALIVE) noOfNeighbours++;
                    if (row < __grid_max_row                         && currGenGrid[row + 1][col]     == ALIVE) noOfNeighbours++;
                    if (row < __grid_max_row && col < __grid_max_col && currGenGrid[row + 1][col + 1] == ALIVE) noOfNeighbours++;

                    if (currGenGrid[row][col] == DEAD && noOfNeighbours == 3)
                    {
                        nextGenGrid[row][col] = ALIVE;
                    }
                    else if (currGenGrid[row][col] == ALIVE && (noOfNeighbours == 2 || noOfNeighbours == 3))
                    {
                        nextGenGrid[row][col] = ALIVE;
                    }
                    else if (currGenGrid[row][col] == ALIVE && (noOfNeighbours < 2 || noOfNeighbours > 3))
                    {
                        nextGenGrid[row][col] = DEAD;
                    }
                    else
                    {
                        nextGenGrid[row][col] = DEAD;
                    }
                }
            }

            // copy next generation grid to the current grid
            for (int i = 0; i < CELLS_IN_GRID_HEIGHT; i++)
            {
                for (int j = 0; j < CELLS_IN_GRID_WIDHT; j++)
                {
                    currGenGrid[i][j] = nextGenGrid[i][j];
                    nextGenGrid[i][j] = DEAD;
                }
            }

            noOfGenerations++;
        }
    }


    CloseWindow();

    return 0;
}
