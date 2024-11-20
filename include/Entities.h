#ifndef ENTITIES_H
#define ENTITIES_H

struct Tree {
    int x;
    int y;
    bool isCut;
    int cutTimer;
};

struct Stone {
    int x;
    int y;
    bool isMined;
};

struct Pig {
    int x;
    int y;
    float targetX;
    float targetY;
    bool isAlive;
};

struct Cow {
    int x;
    int y;
    float targetX;
    float targetY;
    bool isAlive;
};


void InitializeTrees();
void InitializeStones();
void InitializePigs();
void InitializeCows();

void UpdatePigs();
void UpdateCows();

void SelectPigs();
void SelectCows();

void DrawTrees();
void DrawStones();
void DrawHouses();
void DrawPigs();
void DrawCows();

#endif // ENTITIES_H
