// Game.h
#ifndef GAME_H
#define GAME_H

#include <vector>
#include "raylib.h"
#include "Dwarf.h"
#include "Entities.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_WIDTH 50    
#define MAP_HEIGHT 50
#define NUMBER_OF_TREE 60
#define NUMBER_OF_STONE 35
#define NUMBER_OF_DWARF 10
#define NUMBER_OF_PIG 10
#define NUMBER_OF_COW 10
#define NUMBER_OF_FACTION 2

struct Notification {
    std::string message;
    int duration;
};

struct Faction {
    float centerX;
    float centerY;
    std::string name;
    Color color;
};

struct House {
    int x;
    int y;
    int factionId;
};

// Variables globales (déclarées)
extern int map[MAP_WIDTH][MAP_HEIGHT];
extern Faction factions[NUMBER_OF_FACTION];
extern std::vector<Notification> notifications;
extern std::vector<House> houses;

extern bool paused;
extern int totalWood;
extern int totalStone;
extern float offsetX;
extern float offsetY;
extern bool dragging;
extern Vector2 dragStart;
extern float zoom;

extern int selectedDwarf;
extern int selectedPig;
extern int selectedCow;


extern int dwarfsFrameCounter;
extern int pigsFrameCounter;
extern int cowsFrameCounter;


extern Dwarf dwarfs[NUMBER_OF_DWARF];
extern Tree trees[NUMBER_OF_TREE];
extern Stone stones[NUMBER_OF_STONE];
extern Pig pigs[NUMBER_OF_PIG];
extern Cow cows[NUMBER_OF_COW];

extern float lastUpdateTime;

void InitializeMap();
void CreateFaction(int factionId, float x, float y, const std::string &name, Color color);
void UpdateCameraMovement();
void DrawMap();
void DrawTotalResources();
void AddNotification(const std::string &message);
void UpdateNotifications();
void DrawNotifications();

#endif // GAME_H
