#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

// Configuration de la fenêtre et de la carte
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAP_WIDTH 50
#define MAP_HEIGHT 50

struct Notification
{
    std::string message;
    int duration; // Durée restante en frames
};

struct Dwarf
{
    float x;
    float y;
    float targetX;
    float targetY;
    int health;
    char name[20];
    int hunger;
    int fatigue;
    int skill;
    int wood;
    bool resting;
    bool hasHouse;
    int houseX;
    int houseY;
    int factionId;
};

struct Faction
{
    float centerX;
    float centerY;
    std::string name;
    Color color;
};

struct Tree
{
    int x;
    int y;
    bool isCut;
    int cutTimer;
};

struct Stone
{
    int x;
    int y;
    bool isMined;
};

struct House
{
    int x;
    int y;
    int factionId;
};


const int NUMBER_OF_TREE = 100;
const int NUMBER_OF_STONE = 15;
const int NUMBER_OF_DWARF = 10;
const int NUMBER_OF_FACTION = 2;
Dwarf dwarfs[NUMBER_OF_DWARF];
Tree trees[NUMBER_OF_TREE];
Stone stones[NUMBER_OF_STONE];
Faction factions[NUMBER_OF_FACTION] = {};
std::vector<House> houses;
int selectedDwarf = -1;
float offsetX = 0.0f;
float offsetY = 0.0f;
int map[MAP_WIDTH][MAP_HEIGHT];
int totalWood = 25;
int totalStone = 0;
float zoom = 1.0f;

int frameCounter = 0;
bool paused = false;
bool dragging = false;
Vector2 dragStart = {0.0f, 0.0f};
std::vector<Notification> notifications;

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

void InitializeDwarfs()
{
    for (int i = 0; i < NUMBER_OF_DWARF; i++)
    {
        dwarfs[i].x = GetRandomValue(1, MAP_WIDTH - 2);
        dwarfs[i].y = GetRandomValue(1, MAP_HEIGHT - 2);
        dwarfs[i].targetX = dwarfs[i].x;
        dwarfs[i].targetY = dwarfs[i].y;
        dwarfs[i].health = 100;
        dwarfs[i].hunger = GetRandomValue(0, 100);
        dwarfs[i].fatigue = GetRandomValue(0, 100);
        dwarfs[i].skill = GetRandomValue(0, 100);
        dwarfs[i].wood = 0;
        dwarfs[i].resting = false;
        dwarfs[i].hasHouse = false;
        dwarfs[i].factionId = -1;
        sprintf(dwarfs[i].name, "Dwarf %d", i + 1);
    }
}

void InitializeTrees()
{
    for (int i = 0; i < NUMBER_OF_TREE; i++)
    {
        trees[i].x = GetRandomValue(1, MAP_WIDTH - 2);
        trees[i].y = GetRandomValue(1, MAP_HEIGHT - 2);
        trees[i].isCut = false;
        trees[i].cutTimer = 0;
    }
}

void InitializeStones()
{
    for (int i = 0; i < NUMBER_OF_STONE; i++)
    {
        stones[i].x = GetRandomValue(1, MAP_WIDTH - 2);
        stones[i].y = GetRandomValue(1, MAP_HEIGHT - 2);
        stones[i].isMined = false;
    }
}

void CreateFaction(int factionId, float x, float y, const std::string &name, Color color)
{
    Faction newFaction;
    newFaction.centerX = x;
    newFaction.centerY = y;
    newFaction.name = name;
    newFaction.color = color;
    factions[factionId] = newFaction;
}

// Dessiner la carte
void DrawMap()
{
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            Color color = (map[x][y] == 1) ? DARKGRAY : LIGHTGRAY;
            Rectangle rect = {(x * 16 + offsetX) * zoom, (y * 16 + offsetY) * zoom, 16 * zoom, 16 * zoom};
            DrawRectanglePro(rect, {0, 0}, 0.0f, color);
        }
    }
}

// Dessiner les nains
void DrawDwarfs()
{
    for (int i = 0; i < NUMBER_OF_DWARF; i++)
    {
        if (dwarfs[i].factionId == -1) continue; // Ne dessine que les nains avec une faction

        Color factionColor = factions[dwarfs[i].factionId].color;
        float dwarfX = (dwarfs[i].x * 16 + 8 + offsetX) * zoom;
        float dwarfY = (dwarfs[i].y * 16 + 8 + offsetY) * zoom;
        DrawCircle(dwarfX, dwarfY, 8 * zoom, factionColor);
        DrawCircle(dwarfX, dwarfY, 6 * zoom, BLACK);
    }
}

void DrawHouses()
{
    for (House house : houses)
    {
        Color houseColor = factions[house.factionId].color;
        DrawRectangle((house.x * 16 + offsetX) * zoom, (house.y * 16 + offsetY) * zoom, 16 * zoom, 16 * zoom, houseColor);
    }
}

void DrawTrees()
{
    for (int i = 0; i < NUMBER_OF_TREE; i++)
    {
        if (!trees[i].isCut)
        {
            DrawCircle((trees[i].x * 16 + 8 + offsetX) * zoom, (trees[i].y * 16 + 8 + offsetY) * zoom, 6 * zoom, GREEN);
        }
    }
}

void DrawStones()
{
    for (int i = 0; i < NUMBER_OF_STONE; i++)
    {
        if (!stones[i].isMined)
        {
            DrawCircle((stones[i].x * 16 + 8 + offsetX) * zoom, (stones[i].y * 16 + 8 + offsetY) * zoom, 6 * zoom, DARKBROWN);
        }
    }
}

// Sélectionner un nain
void SelectDwarf()
{
    for (int i = 0; i < NUMBER_OF_DWARF; i++)
    {
        float dwarfX = (dwarfs[i].x * 16 + 8 + offsetX) * zoom;
        float dwarfY = (dwarfs[i].y * 16 + 8 + offsetY) * zoom;
        if (CheckCollisionPointCircle(GetMousePosition(), {dwarfX, dwarfY}, 8 * zoom))
        {
            selectedDwarf = i;
            break;
        }
    }
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

void DrawNotifications()
{
    int yOffset = 50;
    for (int i = 0; i < notifications.size(); i++)
    {
        DrawText(notifications[i].message.c_str(), 10, yOffset, 20, BLACK);
        yOffset += 20;
    }
}

// Mettre à jour les nains
void UpdateDwarfs()
{
    if (paused) return;

    frameCounter++;
    if (frameCounter >= 60)
    {
        for (int i = 0; i < NUMBER_OF_DWARF; i++)
        {
            Dwarf &dwarf = dwarfs[i];
            if (!dwarf.hasHouse && totalWood >= 1)
            {
                dwarf.houseX = dwarf.x;
                dwarf.houseY = dwarf.y;
                dwarf.hasHouse = true;
                houses.push_back({dwarf.houseX, dwarf.houseY, dwarf.factionId});
                totalWood -= 1;
                AddNotification(std::string(dwarf.name) + " a construit une maison.");

                if (dwarf.factionId == -1 && houses.size() <= NUMBER_OF_FACTION)
                {
                    dwarf.factionId = houses.size() - 1;
                    std::string factionName = "Faction " + std::to_string(dwarf.factionId);
                    Color factionColor = (dwarf.factionId % 2 == 0) ? RED : BLUE;
                    CreateFaction(dwarf.factionId, dwarf.houseX, dwarf.houseY, factionName, factionColor);
                }
            }

            if (dwarf.resting)
            {
                float speed = 1.0f;
                if (std::abs(dwarf.x - dwarf.houseX) > 0.1f || std::abs(dwarf.y - dwarf.houseY) > 0.1f)
                {
                    if (dwarf.x < dwarf.houseX) dwarf.x += speed;
                    else if (dwarf.x > dwarf.houseX) dwarf.x -= speed;
                    if (dwarf.y < dwarf.houseY) dwarf.y += speed;
                    else if (dwarf.y > dwarf.houseY) dwarf.y -= speed;
                }
                else
                {
                    dwarf.fatigue -= 5;
                    if (dwarf.fatigue < 0) dwarf.fatigue = 0;
                    if (dwarf.fatigue <= 20)
                    {
                        dwarf.resting = false;
                        AddNotification(std::string(dwarf.name) + " a terminé son repos.");
                    }
                }
            }
            else
            {
                if (dwarf.factionId != -1)
                {
                    float distanceToCenter = sqrt(pow(dwarf.x - factions[dwarf.factionId].centerX, 2) +
                                                  pow(dwarf.y - factions[dwarf.factionId].centerY, 2));
                    if (distanceToCenter > 10)
                    {
                        if (dwarf.x < factions[dwarf.factionId].centerX) dwarf.x++;
                        if (dwarf.x > factions[dwarf.factionId].centerX) dwarf.x--;
                        if (dwarf.y < factions[dwarf.factionId].centerY) dwarf.y++;
                        if (dwarf.y > factions[dwarf.factionId].centerY) dwarf.y--;
                    }
                    else
                    {
                        int direction = GetRandomValue(0, 3);
                        if (direction == 0 && dwarf.y > 1) dwarf.y--;
                        if (direction == 1 && dwarf.y < MAP_HEIGHT - 2) dwarf.y++;
                        if (direction == 2 && dwarf.x > 1) dwarf.x--;
                        if (direction == 3 && dwarf.x < MAP_WIDTH - 2) dwarf.x++;
                        dwarf.hunger += 0.5;
                        if (dwarf.hunger > 100) dwarf.hunger = 100;
                        dwarf.fatigue += 2;
                        if (dwarf.fatigue > 100) dwarf.fatigue = 100;
                    }
                }
            }
        }
        frameCounter = 0;
    }
}

// Dessiner les informations sur le nain sélectionné
void DrawDwarfInfo()
{
    if (selectedDwarf != -1)
    {
        Dwarf dwarf = dwarfs[selectedDwarf];
        DrawText(TextFormat("Name: %s", dwarf.name), 10, SCREEN_HEIGHT - 160, 20, BLACK);
        DrawText(TextFormat("Health: %d", dwarf.health), 10, SCREEN_HEIGHT - 140, 20, BLACK);
        DrawText(TextFormat("Hunger: %d", dwarf.hunger), 10, SCREEN_HEIGHT - 120, 20, BLACK);
        DrawText(TextFormat("Fatigue: %d", dwarf.fatigue), 10, SCREEN_HEIGHT - 100, 20, BLACK);
        DrawText(TextFormat("Skill: %d", dwarf.skill), 10, SCREEN_HEIGHT - 80, 20, BLACK);
        DrawText(TextFormat("Resting: %s", dwarf.resting ? "Yes" : "No"), 10, SCREEN_HEIGHT - 60, 20, BLACK);
        DrawText(TextFormat("Faction: %s", factions[dwarf.factionId].name.c_str()), 10, SCREEN_HEIGHT - 40, 20, BLACK);
    }
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

void DrawTotalResources()
{
    DrawText(TextFormat("Total Wood: %d", totalWood), SCREEN_WIDTH / 2 - 60, 10, 20, DARKGREEN);
    DrawText(TextFormat("Total Stone: %d", totalStone), SCREEN_WIDTH / 2 - 60, 30, 20, DARKBROWN);
}



// Fonction principale
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dwarf Fortress-like with Raylib");
    SetTargetFPS(60);

    InitializeMap();
    InitializeDwarfs();
    InitializeTrees();
    InitializeStones();

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE)) paused = !paused;
        UpdateCameraMovement();
        UpdateDwarfs();
        UpdateNotifications();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawMap();
        DrawTrees();
        DrawStones();
        DrawHouses();
        DrawDwarfs();
        DrawDwarfInfo();
        DrawTotalResources();
        DrawNotifications();
        if (paused) DrawText("PAUSE", SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 20, 40, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}