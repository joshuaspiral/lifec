#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include "raylib.h"
#define SIZE 1000
#define SCREEN_LENGTH 1440

using namespace std;

const int cellSize = SCREEN_LENGTH / SIZE;
const float padding = cellSize * 0.08;
int grid[SIZE * SIZE];

typedef struct {
    double StartTime;
    double Lifetime;
} Timer;

void StartTimer(Timer* timer, double lifetime) {
    timer->StartTime = GetTime();
    timer->Lifetime = lifetime;
}

bool TimerDone(Timer timer) {
    return GetTime() - timer.StartTime >= timer.Lifetime;
}

void clearGrid(int* grid, int size) {
    memset(grid, 0, size);
}

void randomiseGrid(int *grid) {
    srand(time(0));
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i*SIZE+j] = rand() % 2;
        }
    }
}

int aliveNeighbours(int *grid, int y, int x) {
    int count = 0;
    if (y != SIZE - 1) {
        count += grid[(y + 1) * SIZE + x];
        if (x != SIZE - 1) {
            count += grid[y * SIZE + x + 1];
            count += grid[(y + 1) * SIZE + x + 1];
        }
        count += grid[(y + 1) * SIZE + x - 1];
    }

    if (y != 0) {
        count += grid[(y - 1) * SIZE + x];
        if (x != 0) {
            count += grid[y * SIZE + x - 1];
            count += grid[(y - 1) * SIZE + x - 1];
        }
        count += grid[(y - 1) * SIZE + x + 1];
    }
    return count;
}

void renderGrid(int *grid) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i * SIZE + j] == 1)
                DrawRectangleRec(Rectangle {i * cellSize + padding, j * cellSize + padding, cellSize - padding, cellSize - padding}, BLACK);
        }
    }
}

void nextGeneration(int *grid) {
    int newGrid[SIZE*SIZE];

    std::copy(&grid[0], &grid[0] + SIZE * SIZE, &newGrid[0]);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int neighboursAlive = aliveNeighbours(grid, i, j);
            if ((neighboursAlive > 3 || (neighboursAlive < 2)) && grid[i * SIZE + j] == 1)
                newGrid[i * SIZE + j] = 0;
            if (neighboursAlive == 3 && grid[i * SIZE + j] == 0)
                newGrid[i * SIZE + j]= 1;
        }
    }
    std::copy(&newGrid[0], &newGrid[0] + SIZE * SIZE, &grid[0]);
}


int main() {
    Camera2D camera;
    camera.offset = (Vector2){SCREEN_LENGTH / 2.0f, SCREEN_LENGTH / 2.0f};
    camera.target = (Vector2){SCREEN_LENGTH / 2.0f, SCREEN_LENGTH / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 10.0f; // Default zoom

    double speed = 0.5;
    bool pause = 1;

    InitWindow(SCREEN_LENGTH, SCREEN_LENGTH, "Conway's Game of Life");
    SetTargetFPS(60);

    Timer speedTimer;
    StartTimer(&speedTimer, speed);

    while (!WindowShouldClose()) {
        Vector2 screenPos = GetScreenToWorld2D(GetMousePosition(), camera);
        int x = screenPos.x;
        int y = screenPos.y;

        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_SPACE)) pause = ((pause == 1) ? 0 : 1);
        if (IsKeyPressed(KEY_R)) randomiseGrid(grid);
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            if (pause)
                grid[(x / cellSize) * SIZE + (y / cellSize)] = ((grid[(y / cellSize) * SIZE + (x / cellSize)] == 1) ? 0 : 1);
            else if (IsKeyDown(KEY_LEFT_CONTROL) || !pause) {
                camera.target.x -= GetMouseDelta().x * 0.5;
                camera.target.y -= GetMouseDelta().y * 0.5;
            }
        }

        if (IsKeyPressed(KEY_A)) camera.zoom = 2.0f;

        if (GetMouseWheelMove()) {
            if (camera.zoom + GetMouseWheelMove() > 1) {
                camera.offset = GetMousePosition();
                camera.zoom += GetMouseWheelMove();
            } else {
                camera.zoom = 1;
            }
        }

        string speedString = "Simulation Delay: " + to_string(speed) + "s";
        char *speedChar = &speedString[0];

        if (IsKeyDown(KEY_RIGHT))
            speed += 0.005;
        if (IsKeyDown(KEY_LEFT))
            speed = ((speed - 0.005 <= 0) ? 0 : speed - 0.005);

        BeginDrawing();
        ClearBackground(WHITE);
        BeginMode2D(camera);
        renderGrid(grid);
        EndMode2D();

        if (IsKeyDown(KEY_C)) {
            clearGrid(grid, SIZE * SIZE * sizeof(int));
            DrawText("Cleared", SCREEN_LENGTH / 2 - (8*10), 0, 20, BLACK);
        }

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
            DrawRectangle(0, 0, 10 * speedString.length(), 20 + 5, Color {0, 0, 0, 255});
            DrawText(speedChar, 0, 0, 20, WHITE);
        }

        if (!pause && TimerDone(speedTimer)) {
            nextGeneration(grid);
            StartTimer(&speedTimer, speed);
        }
        if (pause) {
            DrawRectangle(SCREEN_LENGTH - 80, 0, (20 * 6) + 5, 20 + 5, Color {0, 0, 0, 255});
            DrawText("Paused", SCREEN_LENGTH - 75, 0, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();


    return 0;
}
