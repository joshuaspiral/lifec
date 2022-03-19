#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <chrono>
#include "raylib.h"
#define HEIGHT 50
#define WIDTH 50
#define SCREEN_HEIGHT 1000
#define SCREEN_WIDTH 1000

using namespace std;

const int column_width = SCREEN_WIDTH / WIDTH;
const int row_height = SCREEN_HEIGHT / HEIGHT;

int grid[HEIGHT][WIDTH];

void resetGrid(int (*grid)[WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = 0;
        }
    }
}

int aliveNeighbours(int (*grid)[WIDTH], int y, int x) {
    int count = 0;
    if (y != HEIGHT - 1) {
        count += grid[y + 1][x];
        if (x != WIDTH - 1) {
            count += grid[y][x + 1];
            count += grid[y + 1][x + 1];
        }
        count += grid[y + 1][x - 1];
    }

    if (y != 0) {
        count += grid[y - 1][x];
        if (x != 0) {
            count += grid[y][x - 1];
            count += grid[y - 1][x - 1];
        }
        count += grid[y - 1][x + 1];
    }
    return count;
}

void renderGrid(int (*grid)[WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (grid[i][j] == 1)
                DrawRectangle(i * row_height, j * column_width, column_width, row_height, BLACK);
        }
    }
}

void nextGeneration(int (*grid)[WIDTH]) {
    int oldgrid[HEIGHT][WIDTH], newGrid[HEIGHT][WIDTH];

    std::copy(&grid[0][0], &grid[0][0] + HEIGHT * WIDTH, &oldgrid[0][0]);
    std::copy(&grid[0][0], &grid[0][0] + HEIGHT * WIDTH, &newGrid[0][0]);
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int neighboursAlive = aliveNeighbours(oldgrid, i, j);
            if ((neighboursAlive > 3 || (neighboursAlive < 2)) && oldgrid[i][j] == 1)
                newGrid[i][j] = 0;
            if (neighboursAlive == 3 && oldgrid[i][j] == 0)
                newGrid[i][j]= 1;
        }
    }
    std::copy(&newGrid[0][0], &newGrid[0][0] + HEIGHT * WIDTH, &grid[0][0]);
}

int main() {
    cout << "Welcome to Conway's Game of Life\nTo reset the grid press R. To pause, press SPACE. To escape, press ESCAPE.\n";
    bool pause = 0;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conway's Game of Life");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_SPACE)) pause = ((pause == 1) ? 0 : 1);
        if (IsKeyPressed(KEY_R)) resetGrid(grid);
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            int x, y;
            x = GetMouseX();
            y = GetMouseY();
            if (x < SCREEN_WIDTH && x >= 0 && y < SCREEN_HEIGHT && y >= 0)
                grid[x / row_height][y / column_width] = ((grid[y / row_height][x / column_width] == 1) ? 0 : 1);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int j = 1; j < WIDTH; j++) {
            DrawLine(column_width * j, 0, column_width * j, SCREEN_HEIGHT, GRAY);
        }
        for (int i = 1; i < HEIGHT; i++) {
            DrawLine(0, row_height * i, SCREEN_WIDTH, row_height * i, GRAY);
        }

        if (!pause) nextGeneration(grid);
        renderGrid(grid);
        EndDrawing();
    }

    CloseWindow();


    return 0;
}
