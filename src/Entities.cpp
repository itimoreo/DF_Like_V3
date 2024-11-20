#include "Entities.h"
#include "raylib.h"
#include "Game.h"
#include <cmath>
#include <iostream>

extern Tree trees[NUMBER_OF_TREE];
extern Stone stones[NUMBER_OF_STONE];
extern Pig pigs[NUMBER_OF_PIG];
extern Cow cows[NUMBER_OF_COW];

extern std::vector<House> houses;
extern float offsetX, offsetY, zoom;
extern int pigsFrameCounter;
extern int cowsFrameCounter;

extern int selectedPig;
extern float lastUpdateTime; // Temps de la dernière mise à jour

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

void InitializePigs() {
    for (int i = 0; i < NUMBER_OF_PIG; i++) {
        pigs[i].x = GetRandomValue(1, MAP_WIDTH - 2);
        pigs[i].y = GetRandomValue(1, MAP_HEIGHT - 2);
        pigs[i].targetX = pigs[i].x; // Initialiser avec la position actuelle
        pigs[i].targetY = pigs[i].y; // Initialiser avec la position actuelle
        pigs[i].isAlive = true;
    }
}

void InitializeCows() {
    for (int i = 0; i < NUMBER_OF_COW; i++) {
        cows[i].x = GetRandomValue(1, MAP_WIDTH - 2);
        cows[i].y = GetRandomValue(1, MAP_HEIGHT - 2);
        cows[i].targetX = cows[i].x; // Initialiser avec la position actuelle
        cows[i].targetY = cows[i].y; // Initialiser avec la position actuelle
        cows[i].isAlive = true;
    }
}


void DrawTrees()
{
    for (int i = 0; i < NUMBER_OF_TREE; i++)
    {
        if (!trees[i].isCut)
        {
            DrawCircle((trees[i].x * 16 + 8 + offsetX) * zoom, (trees[i].y * 16 + 8 + offsetY) * zoom, 6 * zoom, DARKGREEN);
        }
    }
}

void DrawStones()
{
    for (int i = 0; i < NUMBER_OF_STONE; i++)
    {
        if (!stones[i].isMined)
        {
            DrawCircle((stones[i].x * 16 + 8 + offsetX) * zoom, (stones[i].y * 16 + 8 + offsetY) * zoom, 6 * zoom, GRAY);
        }
    }
}

void DrawPigs()
{
    for (int i = 0; i < NUMBER_OF_PIG; i++)
    {
        if (pigs[i].isAlive)
        {
            DrawCircle((pigs[i].x * 16 + 8 + offsetX) * zoom, (pigs[i].y * 16 + 8 + offsetY) * zoom, 6 * zoom, PINK);
            DrawText("Pig", (pigs[i].x * 16 + 8 + offsetX) * zoom - 20, (pigs[i].y * 16 + 8 + offsetY) * zoom - 20, 10, PINK);
        }
    }
}

void DrawCows()
{
    for (int i = 0; i < NUMBER_OF_COW; i++)
    {
        if (cows[i].isAlive)
        {
            DrawCircle((cows[i].x * 16 + 8 + offsetX) * zoom, (cows[i].y * 16 + 8 + offsetY) * zoom, 6 * zoom, BROWN);
            DrawText("Cow", (cows[i].x * 16 + 8 + offsetX) * zoom - 20, (cows[i].y * 16 + 8 + offsetY) * zoom - 20, 10, BROWN);
        }
    }
}

void DrawHouses()
{
    for (House house : houses)
    {
        // Affiche toutes les maisons, même celles sans faction
        Color houseColor = (house.factionId >= 0 && house.factionId < NUMBER_OF_FACTION) ? factions[house.factionId].color : YELLOW;
        DrawRectangle(
            (house.x * 16 + offsetX) * zoom,
            (house.y * 16 + offsetY) * zoom,
            16 * zoom,
            16 * zoom,
            houseColor);
    }
}

void UpdatePigs() {
    if (paused) return;

    float currentTime = GetTime();
    float elapsedTime = currentTime - lastUpdateTime;
    lastUpdateTime = currentTime;

    for (int i = 0; i < NUMBER_OF_PIG; i++) {
        Pig &pig = pigs[i];

        // Gestion du déplacement
        float directionX = pig.targetX - pig.x;
        float directionY = pig.targetY - pig.y;
        float distance = sqrt(directionX * directionX + directionY * directionY);

        if (distance > 0.1f) {
            // Normaliser la direction
            directionX /= distance;
            directionY /= distance;

            // Ajuster la vitesse pour un mouvement naturel
            float adjustedSpeed = 0.5f * 60.0f; // Ajustez la vitesse (~0.5 unités/s)
            pig.x += directionX * adjustedSpeed * elapsedTime;
            pig.y += directionY * adjustedSpeed * elapsedTime;
        } else {
            // Si la cible est atteinte, assigner une nouvelle cible aléatoire
            pig.targetX = GetRandomValue(1, MAP_WIDTH - 2);
            pig.targetY = GetRandomValue(1, MAP_HEIGHT - 2);
        }
    }
}




void UpdateCows() {
    if (paused) return;

    float currentTime = GetTime();
    float elapsedTime = currentTime - lastUpdateTime;
    lastUpdateTime = currentTime;

    for (int i = 0; i < NUMBER_OF_COW; i++) {
        Cow &cow = cows[i];

        // Gestion du déplacement
        float directionX = cow.targetX - cow.x;
        float directionY = cow.targetY - cow.y;
        float distance = sqrt(directionX * directionX + directionY * directionY);

        if (distance > 0.1f) {
            // Normaliser la direction
            directionX /= distance;
            directionY /= distance;

            // Ajuster la vitesse pour un mouvement naturel
            float adjustedSpeed = 0.4f * 60.0f; // Ajustez la vitesse (~0.4 unités/s)
            cow.x += directionX * adjustedSpeed * elapsedTime;
            cow.y += directionY * adjustedSpeed * elapsedTime;
        } else {
            // Si la cible est atteinte, assigner une nouvelle cible aléatoire
            cow.targetX = GetRandomValue(1, MAP_WIDTH - 2);
            cow.targetY = GetRandomValue(1, MAP_HEIGHT - 2);
        }
    }
}







void SelectPigs()
{
    Vector2 mousePos = GetMousePosition();

    // Ajuste la position de la souris pour tenir compte du zoom et des offsets
    float adjustedMouseX = (mousePos.x / zoom) - offsetX;
    float adjustedMouseY = (mousePos.y / zoom) - offsetY;

    for (int i = 0; i < NUMBER_OF_PIG; i++)
    {
        // Calcul correct de la position du cochon en coordonnées de la carte
        float pigX = pigs[i].x * 16 + 8;
        float pigY = pigs[i].y * 16 + 8;

        // Rayon de collision ajusté
        float radius = 8;

        // Vérifie si la position ajustée de la souris est dans le cercle du cochon
        if (CheckCollisionPointCircle({adjustedMouseX, adjustedMouseY}, {pigX, pigY}, radius))
        {
            selectedPig = i;
            return; // Quitte la fonction une fois qu'un cochon est sélectionné
        }
    }

    // Si aucun cochon n'est sélectionné
    selectedPig = -1;
}