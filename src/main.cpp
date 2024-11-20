#include "raylib.h"
#include "Dwarf.h"
#include "Entities.h"
#include "Game.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dwarf Fortress-like with Raylib");
    SetTargetFPS(60);

    InitializeMap();
    InitializeDwarfs();
    InitializeTrees();
    InitializeStones();
    InitializePigs();
    InitializeCows();

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
            paused = !paused;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            SelectDwarf();
            SelectPigs();
        }

        UpdateCameraMovement();
        UpdateDwarfs();
        UpdatePigs();
        UpdateCows();
        UpdateNotifications();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawMap();
        DrawTrees();
        DrawStones();
        DrawHouses();
        DrawDwarfs();
        DrawPigs();
        DrawCows();
        DrawDwarfInfo();
        DrawTotalResources();
        DrawNotifications();
        if (paused)
            DrawText("PAUSE", SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 20, 40, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
