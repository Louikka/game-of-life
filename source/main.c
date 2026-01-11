#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "raylib.h"

#include "timer.h"


/** In pixels. */
#define CELL_SIZE 10

#define CELLS_IN_CHUNK 8
#define CHUNK_SIZE (CELLS_IN_CHUNK*CELL_SIZE)


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 640

#define CELLS_IN_GRID_WIDTH ((int)SCREEN_WIDTH/(int)CELL_SIZE)
#define CELLS_IN_GRID_HEIGHT ((int)SCREEN_HEIGHT/(int)CELL_SIZE)
#define TOTAL_CELLS_IN_GRID (CELLS_IN_GRID_WIDTH*CELLS_IN_GRID_HEIGHT)

/** Alias for `CELLS_IN_GRID_HEIGHT`. */
#define ROWS_IN_GRID CELLS_IN_GRID_HEIGHT
/** Alias for `CELLS_IN_GRID_WIDTH`. */
#define COLS_PER_ROW CELLS_IN_GRID_WIDTH

#define GRID_MAX_ROW (ROWS_IN_GRID-1)
#define GRID_MAX_COL (COLS_PER_ROW-1)

#define CHUNKS_PER_ROW ((int)(COLS_PER_ROW/CELLS_IN_CHUNK))


typedef enum {
    DEAD,
    ALIVE,
} CellState;


void SetGridCellState(uint8_t *grid, int row, int col, CellState state)
{
    uint8_t *chunk = grid + (row * CHUNKS_PER_ROW) + (int)(col / CELLS_IN_CHUNK);

    uint8_t mask = 0x80 >> (col % CELLS_IN_CHUNK);

    if (state == ALIVE)
    {
        *chunk |= mask;
    }
    else
    {
        *chunk &= ~mask;
    }
}

bool IsCellAlive(uint8_t *grid, int row, int col)
{
    uint8_t chunk = *(grid + (row * CHUNKS_PER_ROW) + (int)(col / CELLS_IN_CHUNK));

    uint8_t mask = 0x80 >> (col % CELLS_IN_CHUNK);

    return (chunk & mask) != 0;
}


void UpdateGrid(uint8_t *currentGrid, uint8_t *nextGrid)
{
    for (int row = 0; row < ROWS_IN_GRID; row++)
    {
        for (int col = 0; col < COLS_PER_ROW; col++)
        {
            // update cell of next grid
            unsigned char noOfNeighbours = 0;

            if (row > 0            && col > 0            && IsCellAlive(currentGrid, row - 1, col - 1)) noOfNeighbours++;
            if (row > 0                                  && IsCellAlive(currentGrid, row - 1, col)    ) noOfNeighbours++;
            if (row > 0            && col < GRID_MAX_COL && IsCellAlive(currentGrid, row - 1, col + 1)) noOfNeighbours++;
            if (                      col > 0            && IsCellAlive(currentGrid, row,     col - 1)) noOfNeighbours++;
            if (                      col < GRID_MAX_COL && IsCellAlive(currentGrid, row,     col + 1)) noOfNeighbours++;
            if (row < GRID_MAX_ROW && col > 0            && IsCellAlive(currentGrid, row + 1, col - 1)) noOfNeighbours++;
            if (row < GRID_MAX_ROW                       && IsCellAlive(currentGrid, row + 1, col)    ) noOfNeighbours++;
            if (row < GRID_MAX_ROW && col < GRID_MAX_COL && IsCellAlive(currentGrid, row + 1, col + 1)) noOfNeighbours++;

            if (
                (!IsCellAlive(currentGrid, row, col) && noOfNeighbours == 3) ||
                (IsCellAlive(currentGrid, row, col) && (noOfNeighbours == 2 || noOfNeighbours == 3))
            )
            {
                SetGridCellState(nextGrid, row, col, ALIVE);
            }
            else
            {
                SetGridCellState(nextGrid, row, col, DEAD);
            }
        }
    }

    // copy next generation grid to the current grid
    for (int i = 0; i < CELLS_IN_GRID_HEIGHT; i++)
    {
        for (int j = 0; j < CELLS_IN_GRID_WIDTH; j++)
        {
            SetGridCellState(currentGrid, i, j, IsCellAlive(nextGrid, i, j) ? ALIVE : DEAD);
            SetGridCellState(nextGrid, i, j, DEAD);
        }
    }
}


int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conway's Game of Life");
    SetTargetFPS(30);


    const unsigned int TOTAL_CHUNKS_IN_GRID = (unsigned int) ceil((double)TOTAL_CELLS_IN_GRID / CELLS_IN_CHUNK);


    uint8_t *GridCurrentGen = (uint8_t *) calloc(TOTAL_CHUNKS_IN_GRID, sizeof(uint8_t));
    printf("Allocated to the current gen grid's first element : %zu byte(s).\n", sizeof(GridCurrentGen[0]));
    if (GridCurrentGen == NULL)
    {
        printf("Memory allocation failed.\n");
        return 0;
    }

    uint8_t *GridNextGen = (uint8_t *) calloc(TOTAL_CHUNKS_IN_GRID, sizeof(uint8_t));
    printf("Allocated to the next gen grid's first element : %zu byte(s).\n", sizeof(GridNextGen[0]));
    if (GridNextGen == NULL)
    {
        printf("Memory allocation failed.\n");
        return 0;
    }


    bool isGameRunning = false;

    const float GAME_SPEED = 0.1f; // seconds beetween each generation
    Timer GameTimer = { 0 };

    unsigned int noOfGenerations = 1;


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


        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePosition = GetMousePosition();

            size_t rowUnderMouse = (int) mousePosition.y / CELL_SIZE;
            size_t colUnderMouse = (int) mousePosition.x / CELL_SIZE;

            SetGridCellState(
                GridCurrentGen,
                rowUnderMouse,
                colUnderMouse,
                IsCellAlive(GridCurrentGen, rowUnderMouse, colUnderMouse) ? DEAD : ALIVE
            );
        }


        BeginDrawing();

            ClearBackground(BLACK);

            DrawText(TextFormat("Generation %u.", noOfGenerations), 10, 10, 20, RAYWHITE);
            DrawText("Press SPACE to play/pause game.", 10, 35, 10, RAYWHITE);

            for (int row = 0; row < ROWS_IN_GRID; row++)
            {
                for (int col = 0; col < COLS_PER_ROW; col++)
                {
                    if (IsCellAlive(GridCurrentGen, row, col))
                    {
                        DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, WHITE);
                    }
                }
            }

            if (isGameRunning && IsTimerDone(&GameTimer))
            {
                UpdateGrid(GridCurrentGen, GridNextGen);

                noOfGenerations++;

                StartTimer(&GameTimer, GAME_SPEED);
            }

        EndDrawing();
    }


    CloseWindow();

    free(GridCurrentGen);
    free(GridNextGen);

    return 0;
}
