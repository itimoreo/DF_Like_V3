#ifndef DWARF_H
#define DWARF_H

#include <string>

struct Dwarf {
    float x;
    float y;
    float targetX;
    float targetY;
    int health;
    char name[20];
    float hunger;
    float fatigue;
    int skill;
    int wood;
    bool resting;
    bool hasHouse;
    int houseX;
    int houseY;
    int factionId;
    float speed;
};


void InitializeDwarfs();
void UpdateDwarfs();
void DrawDwarfs();
void DrawDwarfInfo();
void SelectDwarf();

void DwarfChopTree(int dwarfIndex);
void DwarfMineStone(int dwarfIndex);
void DwarfHuntPig(int dwarfIndex);
void DwarfHuntCow(int dwarfIndex);

#endif // DWARF_H
