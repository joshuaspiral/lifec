#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <stack>
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

void renderGrid(int *grid, int originX, int originY, int boundaryX, int boundaryY) {
    for (int i = originX; i < boundaryX; i++) {
        for (int j = originY; j < boundaryY; j++) {
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

int inBounds(int n) {
    if (n >= SIZE)
        return SIZE;
    else if (n < 0)
        return 0;
    else
        return n;
}


int main() {
    Camera2D camera;
    camera.offset = (Vector2){SCREEN_LENGTH / 2.0f, SCREEN_LENGTH / 2.0f};
    camera.target = (Vector2){SCREEN_LENGTH / 2.0f, SCREEN_LENGTH / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 20.0f; // Default zoom

    double speed = 0.5;
    bool pause = 1;

    stack<Vector2> lastMoves;

    InitWindow(SCREEN_LENGTH, SCREEN_LENGTH, "Conway's Game of Life");
    SetTargetFPS(1000);

    Timer speedTimer;
    StartTimer(&speedTimer, speed);


    while (!WindowShouldClose()) {
        Vector2 screenPos = GetScreenToWorld2D(GetMousePosition(), camera);
        int x = screenPos.x;
        int y = screenPos.y;
        int boardY = x / cellSize;
        int boardX = y / cellSize;
        Vector2 origin = GetScreenToWorld2D(Vector2 {0,0}, camera);
        int originX = inBounds(origin.x / cellSize);
        int originY = inBounds(origin.y / cellSize);
        Vector2 boundary = GetScreenToWorld2D(Vector2 {SCREEN_LENGTH, SCREEN_LENGTH}, camera);
        int boundaryX = inBounds(boundary.x / cellSize);
        int boundaryY = inBounds(boundary.y / cellSize);

        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q)) break;
        if (IsKeyPressed(KEY_SPACE)) pause = ((pause == 1) ? 0 : 1);
        if (IsKeyPressed(KEY_R)) randomiseGrid(grid);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (pause) {
                grid[boardY * SIZE + boardX] = ((grid[boardY * SIZE + boardX] == 1) ? 0 : 1);
                lastMoves.push(Vector2 {(float)boardX, (float)boardY});
            } else {
                if (camera.offset.x + GetMouseDelta().x > 0)
                    camera.offset.x += GetMouseDelta().x * 0.5;
                if (camera.offset.y + GetMouseDelta().y > 0)
                    camera.offset.y += GetMouseDelta().y * 0.5;
            }
        }
        if (IsKeyDown(KEY_Z) && !lastMoves.empty()) {
            Vector2 lastMove = lastMoves.top();
            grid[(int)(lastMove.y * SIZE + lastMove.x)] = ((grid[(int)(lastMove.y * SIZE + lastMove.x)] == 1) ? 0 : 1);
            lastMoves.pop();
        }

        if (IsKeyPressed(KEY_A)) camera.zoom = 5.0f;
            

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

        if (IsKeyPressed(KEY_RIGHT))
            speed += 0.1;
        if (IsKeyPressed(KEY_LEFT))
            speed = ((speed - 0.1 <= 0) ? 0 : speed - 0.1);

        BeginDrawing();
        ClearBackground(WHITE);
        BeginMode2D(camera);
        renderGrid(grid, originX, originY, boundaryX, boundaryY);
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
