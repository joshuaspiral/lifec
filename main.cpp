// Refactor code TODO 
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <chrono>
#include "raylib.h"
#define HEIGHT 1000
#define WIDTH 100
#define SCREEN_HEIGHT 1000

using namespace std;

const int cellSize = SCREEN_HEIGHT / WIDTH;
const int padding = cellSize * 0.10;

int grid[HEIGHT][WIDTH];

void clearGrid(int (*grid)[WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = 0;
        }
    }
}

void randomiseGrid(int (*grid)[WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = rand() % 2;
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
                DrawRectangle(i * cellSize + padding, j * cellSize + padding, cellSize - padding, cellSize - padding, BLACK);
        }
    }
}

void nextGeneration(int (*grid)[WIDTH], int speed) {
    int newGrid[HEIGHT][WIDTH];

    std::copy(&grid[0][0], &grid[0][0] + HEIGHT * WIDTH, &newGrid[0][0]);
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int neighboursAlive = aliveNeighbours(grid, i, j);
            if ((neighboursAlive > 3 || (neighboursAlive < 2)) && grid[i][j] == 1)
                newGrid[i][j] = 0;
            if (neighboursAlive == 3 && grid[i][j] == 0)
                newGrid[i][j]= 1;
        }
    }
    std::copy(&newGrid[0][0], &newGrid[0][0] + HEIGHT * WIDTH, &grid[0][0]);
    usleep(speed);
}

int main() {
    int speed = 100000; // 100ms in microseconds
    bool pause = 1;
    srand(time(0));
    cout << "Welcome to Conway's Game of Life\nTo clear the grid press C. To randomise the grid, press R. To pause, press SPACE. To increase simulation delay, press RIGHT arrow. To decrease, press LEFT arrow. To escape, press ESCAPE.\n";
    InitWindow(SCREEN_HEIGHT, SCREEN_HEIGHT, "Conway's Game of Life");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_SPACE)) {
            pause = ((pause == 1) ? 0 : 1);
        }
        if (IsKeyPressed(KEY_C)) clearGrid(grid);
        if (IsKeyPressed(KEY_R)) randomiseGrid(grid);
        if (IsKeyDown(KEY_RIGHT)) {
            speed += 1000;
            string speedString = "Simulation Delay: " + to_string(speed / 1000) + "ms";
            char *speedChar = &speedString[0];
            DrawRectangle(0, 0, 10 * speedString.length(), 20 + 5, Color {0, 0, 0, 150});
            DrawText(speedChar, 0, 0, 20, RED);
        }
        if (IsKeyDown(KEY_LEFT)) {
            speed = ((speed - 1000 <= 0) ? 0 : speed - 1000);
            string speedString = "Simulation Delay: " + to_string(speed / 1000) + "ms";
            char *speedChar = &speedString[0];
            DrawRectangle(0, 0, 10 * speedString.length(), 20 + 5, Color {0, 0, 0, 150});
            DrawText(speedChar, 0, 0, 20, RED);
        };
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            int x, y;
            x = GetMouseX();
            y = GetMouseY();
            if (x < SCREEN_HEIGHT && x >= 0 && y < SCREEN_HEIGHT && y >= 0)
                grid[x / cellSize][y / cellSize] = ((grid[y / cellSize][x / cellSize] == 1) ? 0 : 1);
        }

        BeginDrawing();
        ClearBackground(WHITE);

        renderGrid(grid);
        if (!pause) nextGeneration(grid, speed);
        else {
            DrawRectangle(SCREEN_HEIGHT - 80, 0, (20 * 6) + 5, 20 + 5, Color {0, 0, 0, 150});
            DrawText("Paused", SCREEN_HEIGHT - 75, 0, 20, RED);
        }
        EndDrawing();
    }

    CloseWindow();


    return 0;
}
