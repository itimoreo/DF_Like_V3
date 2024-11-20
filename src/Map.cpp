#include "Map.h"
#include "raylib.h"
#include "Game.h"

extern int map[MAP_WIDTH][MAP_HEIGHT];
extern float offsetX, offsetY, zoom;

// Initialisation de la carte avec des bordures de mur
void InitializeMap()
{
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            map[x][y] = (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) ? 1 : 0;
        }
    }
}

// Dessiner la carte
void DrawMap()
{
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            Color color = (map[x][y] == 1) ? DARKGRAY : GREEN;
            Rectangle rect = {(x * 16 + offsetX) * zoom, (y * 16 + offsetY) * zoom, 16 * zoom, 16 * zoom};
            DrawRectanglePro(rect, {0, 0}, 0.0f, color);
        }
    }
}