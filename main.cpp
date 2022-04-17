#include "raylib.h"
#include <iostream>
#include <stack>
#include <string.h>
#define SIZE 800
#define SCREEN_LENGTH 800

using namespace std;

const int cellSize = SCREEN_LENGTH / SIZE;
const float padding = cellSize * 0.08;
bool grid[SIZE * SIZE];

typedef struct {
    double StartTime;
    double Lifetime;
} Timer;

void StartTimer(Timer *timer, double lifetime) {
    timer->StartTime = GetTime();
    timer->Lifetime = lifetime;
}

bool TimerDone(Timer timer) {
    return GetTime() - timer.StartTime >= timer.Lifetime;
}

void clearGrid(bool *grid, int size) { memset(grid, 0, size); }

void randomiseGrid(bool *grid) {
    srand(time(0));
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i * SIZE + j] = rand() % 2;
        }
    }
}

int aliveNeighbours(bool *grid, int y, int x) {
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

void renderGrid(bool *grid, int originX, int originY, int boundaryX,
        int boundaryY) {
    for (int i = originX; i < boundaryX; i++) {
        for (int j = originY; j < boundaryY; j++) {
            if (grid[i * SIZE + j] == 1)
                DrawRectangleRec(Rectangle{i * cellSize + padding,
                        j * cellSize + padding, cellSize - padding,
                        cellSize - padding},
                        BLACK);
        }
    }
}

void nextGeneration(bool *grid) {
    bool newGrid[SIZE * SIZE];

    std::copy(&grid[0], &grid[0] + SIZE * SIZE, &newGrid[0]);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int neighboursAlive = aliveNeighbours(grid, i, j);
            if ((neighboursAlive > 3 || (neighboursAlive < 2)) &&
                    grid[i * SIZE + j] == 1)
                newGrid[i * SIZE + j] = 0;
            if (neighboursAlive == 3 && grid[i * SIZE + j] == 0)
                newGrid[i * SIZE + j] = 1;
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

    double speed = 0.25;
    bool pause = 1;

    stack<int> lastMoves;

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
        Vector2 origin = GetScreenToWorld2D(Vector2{0, 0}, camera);
        int originX = inBounds(origin.x / cellSize);
        int originY = inBounds(origin.y / cellSize);
        Vector2 boundary =
            GetScreenToWorld2D(Vector2{SCREEN_LENGTH, SCREEN_LENGTH}, camera);
        int boundaryX = inBounds(boundary.x / cellSize);
        int boundaryY = inBounds(boundary.y / cellSize);

        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q))
            break;
        if (IsKeyPressed(KEY_SPACE))
            pause = ((pause == 1) ? 0 : 1);
        if (IsKeyPressed(KEY_R))
            randomiseGrid(grid);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (pause) {
                int index = boardY * SIZE + boardX;
                if (index > 0 || index <= SIZE * SIZE) {
                    grid[boardY * SIZE + boardX] =
                        ((grid[boardY * SIZE + boardX] == 1) ? 0 : 1);
                    lastMoves.push(index);
                }
            } else {
                int newX = camera.offset.x + GetMouseDelta().x * 0.5;
                int newY = camera.offset.y + GetMouseDelta().y * 0.5;
                if (newX >= 0 && newX <= SIZE)
                    camera.offset.x = newX;
                if (newY >= 0 && newY <= SIZE)
                    camera.offset.y = newY;
            }
        }

        if (IsKeyDown(KEY_H)) if (camera.offset.x + 1 > 0) camera.offset.x++;
        if (IsKeyDown(KEY_J)) if (camera.offset.y - 1 > 0) camera.offset.y--;
        if (IsKeyDown(KEY_K)) if (camera.offset.y + 1 > 0) camera.offset.y++;
        if (IsKeyDown(KEY_L)) if (camera.offset.x - 1 > 0) camera.offset.x--;
        
        if (IsKeyDown(KEY_Z) && !lastMoves.empty()) {
            int lastMove = lastMoves.top();
            grid[lastMove] = (grid[lastMove] == 1) ? 0 : 1;
            lastMoves.pop();
        }

        if (IsKeyPressed(KEY_A))
            camera.zoom = 5.0f;

        if (GetMouseWheelMove() || IsKeyDown(KEY_MINUS) || IsKeyDown(KEY_EQUAL)) {
            if (GetMouseWheelMove()) {
                if (camera.zoom + GetMouseWheelMove() >= 1) {
                    camera.offset = GetMousePosition();
                    camera.zoom += GetMouseWheelMove() * camera.zoom;
                } else {
                    camera.zoom = 1;
                }
            }
            if (IsKeyDown(KEY_EQUAL)) {
                if (camera.zoom + 0.02 * camera.zoom <= SIZE)
                    camera.zoom += 0.02 * camera.zoom;
            }
            if (IsKeyDown(KEY_MINUS)) {
                if (camera.zoom - 0.02 * camera.zoom >= 1)
                    camera.zoom -= 0.02 * camera.zoom;
            }
        }

        string speedString = "Simulation Delay: " + to_string(speed) + "s";
        char *speedChar = &speedString[0];

        if (IsKeyPressed(KEY_RIGHT))
            speed += 0.025;
        if (IsKeyPressed(KEY_LEFT))
            speed = ((speed - 0.025 <= 0) ? 0 : speed - 0.025);

        BeginDrawing();
        ClearBackground(WHITE);
        BeginMode2D(camera);
        renderGrid(grid, originX, originY, boundaryX, boundaryY);
        EndMode2D();

        if (IsKeyDown(KEY_C)) {
            clearGrid(grid, SIZE * SIZE * sizeof(int));
            DrawText("Cleared", SCREEN_LENGTH / 2 - (8 * 10), 0, 20, BLACK);
        }

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
            DrawRectangle(0, 0, 10 * speedString.length(), 20 + 5,
                    Color{0, 0, 0, 255});
            DrawText(speedChar, 0, 0, 20, WHITE);
        }

        if (!pause && TimerDone(speedTimer)) {
            nextGeneration(grid);
            StartTimer(&speedTimer, speed);
        }
        if (pause) {
            DrawRectangle(SCREEN_LENGTH - 80, 0, (20 * 6) + 5, 20 + 5,
                    Color{0, 0, 0, 255});
            DrawText("Paused", SCREEN_LENGTH - 75, 0, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
