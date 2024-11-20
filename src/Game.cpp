// Game.cpp
#include "Game.h"
#include "Dwarf.h"
#include "Entities.h"
#include "raylib.h"
#include <iostream>
#include <cmath>

// Définition des variables globales
int map[MAP_WIDTH][MAP_HEIGHT] = {0};
Faction factions[NUMBER_OF_FACTION] = {};
std::vector<Notification> notifications;
std::vector<House> houses;
bool paused = false;
int totalWood = 25;
int totalStone = 0;
float offsetX = 0.0f;
float offsetY = 0.0f;
bool dragging = false;
Vector2 dragStart = {0.0f, 0.0f};
float zoom = 1.0f;

int selectedDwarf = -1;
int selectedPig = -1;
int selectedCow = -1;

int dwarfsFrameCounter = 0;
int pigsFrameCounter = 0;
int cowsFrameCounter = 0;


Dwarf dwarfs[NUMBER_OF_DWARF];
Tree trees[NUMBER_OF_TREE];
Stone stones[NUMBER_OF_STONE];
Pig pigs[NUMBER_OF_PIG];
Cow cows[NUMBER_OF_COW];

float lastUpdateTime = GetTime(); // Temps de la dernière mise à jour


void CreateFaction(int factionId, float x, float y, const std::string &name, Color color)
{
    Faction newFaction;
    newFaction.centerX = x;
    newFaction.centerY = y;
    newFaction.name = name;
    newFaction.color = color;
    factions[factionId] = newFaction;
}


// Mettre à jour la caméra
void UpdateCameraMovement()
{
    if (GetMouseWheelMove() != 0)
    {
        float zoomIncrement = 0.1f;
        zoom += GetMouseWheelMove() * zoomIncrement;
        if (zoom < 0.5f) zoom = 0.5f;
        if (zoom > 2.0f) zoom = 2.0f;
    }

    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
    {
        dragging = true;
        dragStart = GetMousePosition();
    }

    if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON))
    {
        dragging = false;
    }

    if (dragging)
    {
        Vector2 mousePosition = GetMousePosition();
        offsetX += (mousePosition.x - dragStart.x) / zoom;
        offsetY += (mousePosition.y - dragStart.y) / zoom;
        dragStart = mousePosition;
    }
}


void DrawTotalResources() {
    // Position du texte
    int x = SCREEN_WIDTH / 2 - 60;
    int y1 = 10;  // Position pour le bois
    int y2 = 30;  // Position pour la pierre

    // Texte avec un contour noir
    for (int offsetX = -1; offsetX <= 1; offsetX++) {
        for (int offsetY = -1; offsetY <= 1; offsetY++) {
            DrawText(TextFormat("Total Wood: %d", totalWood), x + offsetX, y1 + offsetY, 20, BLACK);
            DrawText(TextFormat("Total Stone: %d", totalStone), x + offsetX, y2 + offsetY, 20, BLACK);
        }
    }

    // Texte principal
    DrawText(TextFormat("Total Wood: %d", totalWood), x, y1, 20, WHITE);
    DrawText(TextFormat("Total Stone: %d", totalStone), x, y2, 20, WHITE);
}


void AddNotification(const std::string &message)
{
    if (notifications.size() > 5)
    {
        notifications.erase(notifications.begin());
    }
    notifications.push_back({message, 180});
}


void UpdateNotifications()
{
    for (int i = 0; i < notifications.size(); i++)
    {
        notifications[i].duration--;
        if (notifications[i].duration <= 0)
        {
            notifications.erase(notifications.begin() + i);
            i--;
        }
    }
}


void DrawNotifications() {
    int yOffset = 50;
    for (int i = 0; i < notifications.size(); i++) {
        // Texte avec un contour noir
        for (int offsetX = -1; offsetX <= 1; offsetX++) {
            for (int offsetY = -1; offsetY <= 1; offsetY++) {
                DrawText(notifications[i].message.c_str(), 10 + offsetX, yOffset + offsetY, 16, BLACK);
            }
        }

        // Texte principal
        DrawText(notifications[i].message.c_str(), 10, yOffset, 16, WHITE);
        yOffset += 20;
    }
}

