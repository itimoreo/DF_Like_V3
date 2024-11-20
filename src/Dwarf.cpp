#include "Dwarf.h"
#include "raylib.h"
#include <cmath>
#include <string>
#include <cfloat>
#include <iostream>
#include "Game.h"
#include "Entities.h"

// Fonction utilitaire pour restreindre les valeurs entre un minimum et un maximum
float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

extern Dwarf dwarfs[NUMBER_OF_DWARF];
extern Faction factions[NUMBER_OF_FACTION];
extern int selectedDwarf;
extern float offsetX, offsetY, zoom;
extern int dwarfsFrameCounter;
extern bool paused;
extern std::vector<House> houses;


extern float lastUpdateTime; // Temps de la dernière mise à jour

void InitializeDwarfs()
{
    for (int i = 0; i < NUMBER_OF_DWARF; i++)
    {
        dwarfs[i].x = static_cast<float>(GetRandomValue(1, MAP_WIDTH - 2));
        dwarfs[i].y = static_cast<float>(GetRandomValue(1, MAP_HEIGHT - 2));
        dwarfs[i].targetX = dwarfs[i].x;
        dwarfs[i].targetY = dwarfs[i].y;
        dwarfs[i].health = 100;

        // Initialisation de faim et fatigue entre 0 et 20
        dwarfs[i].hunger = static_cast<float>(GetRandomValue(0, 40));
        dwarfs[i].fatigue = static_cast<float>(GetRandomValue(0, 40));

        dwarfs[i].skill = GetRandomValue(0, 5);
        dwarfs[i].wood = 0;
        dwarfs[i].resting = false;
        dwarfs[i].hasHouse = false;
        dwarfs[i].factionId = GetRandomValue(0, NUMBER_OF_FACTION - 1);

        dwarfs[i].speed = 0.05f;

        snprintf(dwarfs[i].name, sizeof(dwarfs[i].name), "Dwarf %d", i + 1);
    }
}

// Faire couper un arbre à un nain
void DwarfChopTree(int dwarfIndex) {
    Dwarf &dwarf = dwarfs[dwarfIndex];

    for (int i = 0; i < NUMBER_OF_TREE; i++) {
        Tree &tree = trees[i];
        
        // Vérifie si l'arbre n'a pas encore été coupé
        if (!tree.isCut) {
            // Calcul de la distance entre le nain et l'arbre
            float distance = sqrt(pow(dwarf.x - tree.x, 2) + pow(dwarf.y - tree.y, 2));
            
            if (distance < 1.5f) { // Si le nain est proche de l'arbre
                tree.isCut = true; // Marque l'arbre comme coupé
                dwarf.wood += 5;   // Ajoute du bois au nain
                totalWood += 5;    // Ajoute du bois aux ressources totales
                AddNotification(std::string(dwarf.name) + " a coupé un arbre et a obtenu 5 bois.");
                return;
            }
        }
    }
}

void DwarfMineRock(int dwarfIndex) {
    Dwarf &dwarf = dwarfs[dwarfIndex];

    for (int i = 0; i < NUMBER_OF_STONE; i++) {
        Stone &stone = stones[i];
        
        // Vérifie si le rocher n'a pas encore été miné
        if (!stone.isMined) {
            // Calcul de la distance entre le nain et le rocher
            float distance = sqrt(pow(dwarf.x - stone.x, 2) + pow(dwarf.y - stone.y, 2));
            
            if (distance < 1.5f) { // Si le nain est proche du rocher
                stone.isMined = true; // Marque le rocher comme miné
                totalStone += 3;      // Ajoute de la pierre aux ressources totales
                AddNotification(std::string(dwarf.name) + " a miné un rocher et a obtenu 3 pierres.");
                return;
            }
        }
    }
}

// Trouver et chasser un cochon
// Inclure la logique pour la chasse avec un mouvement fluide
bool DwarfHuntPigSuccess(int dwarfIndex)
{
    Dwarf &dwarf = dwarfs[dwarfIndex];
    Pig *closestPig = nullptr;
    float minDistance = FLT_MAX;

    // Trouver le cochon vivant le plus proche
    for (int i = 0; i < NUMBER_OF_PIG; i++)
    {
        Pig &pig = pigs[i];
        if (pig.isAlive)
        {
            float distance = sqrt(pow(dwarf.x - pig.x, 2) + pow(dwarf.y - pig.y, 2));
            if (distance < minDistance)
            {
                minDistance = distance;
                closestPig = &pig;
            }
        }
    }

    // Si un cochon est trouvé et est assez proche pour être chassé
    if (closestPig && minDistance < 1.5f)
    {
        closestPig->isAlive = false; // Marque le cochon comme mort
        dwarf.hunger -= 80; // Réduit la faim du nain
        dwarf.hunger = Clamp(dwarf.hunger, 0.0f, 100.0f); // Restreindre la faim après réduction
        AddNotification(std::string(dwarf.name) + " a chassé un cochon et a réduit sa faim.");
        return true;
    }

    // Si un cochon est trouvé mais est loin, le nain se déplace progressivement vers lui
    if (closestPig)
    {
        // Calculer le vecteur de direction
        float directionX = closestPig->x - dwarf.x;
        float directionY = closestPig->y - dwarf.y;
        float magnitude = sqrt(directionX * directionX + directionY * directionY);

        // Déplacer le nain progressivement selon sa vitesse
        if (magnitude > 0)
        {
            dwarf.x += (directionX / magnitude) * dwarf.speed;
            dwarf.y += (directionY / magnitude) * dwarf.speed;
        }
    }

    return false;
}

bool DwarfHuntCowSuccess(int dwarfIndex)
{
    Dwarf &dwarf = dwarfs[dwarfIndex];
    Cow *closestCow = nullptr;
    float minDistance = FLT_MAX;

    // Trouver la vache vivante la plus proche
    for (int i = 0; i < NUMBER_OF_COW; i++)
    {
        Cow &cow = cows[i];
        if (cow.isAlive)
        {
            float distance = sqrt(pow(dwarf.x - cow.x, 2) + pow(dwarf.y - cow.y, 2));
            if (distance < minDistance)
            {
                minDistance = distance;
                closestCow = &cow;
            }
        }
    }

    // Si une vache est trouvée et est assez proche pour être chassée
    if (closestCow && minDistance < 1.5f)
    {
        closestCow->isAlive = false; // Marque la vache comme morte
        dwarf.hunger -= 80; // Réduit la faim du nain
        dwarf.hunger = Clamp(dwarf.hunger, 0.0f, 100.0f); // Restreindre la faim après réduction
        AddNotification(std::string(dwarf.name) + " a chassé une vache et a réduit sa faim.");
        return true;
    }

    // Si une vache est trouvée mais est loin, le nain se déplace progressivement vers elle
    if (closestCow)
    {
        // Calculer le vecteur de direction
        float directionX = closestCow->x - dwarf.x;
        float directionY = closestCow->y - dwarf.y;
        float magnitude = sqrt(directionX * directionX + directionY * directionY);

        // Déplacer le nain progressivement selon sa vitesse
        if (magnitude > 0)
        {
            dwarf.x += (directionX / magnitude) * dwarf.speed;
            dwarf.y += (directionY / magnitude) * dwarf.speed;
        }
    }

    return false;
}



void UpdateDwarfs() {
    if (paused) return;

    float currentTime = GetTime();
    float elapsedTime = currentTime - lastUpdateTime;
    lastUpdateTime = currentTime;

    for (int i = 0; i < NUMBER_OF_DWARF; i++) {
        Dwarf &dwarf = dwarfs[i];

        if (dwarf.resting) {
            // Gestion du repos
            dwarf.fatigue -= 5.0f * elapsedTime;
            dwarf.fatigue = Clamp(dwarf.fatigue, 0.0f, 100.0f);
            if (dwarf.fatigue <= 10.0f) {
                dwarf.resting = false;
                AddNotification(std::string(dwarf.name) + " a terminé son repos.");
            }
        } else {
            // Gestion du déplacement
            float directionX = dwarf.targetX - dwarf.x;
            float directionY = dwarf.targetY - dwarf.y;
            float distance = sqrt(directionX * directionX + directionY * directionY);

            if (distance > 0.1f) {
                // Normaliser la direction
                directionX /= distance;
                directionY /= distance;

                // Ajuster la vitesse pour un mouvement naturel
                float adjustedSpeed = dwarf.speed * 60.0f; // Échelle pour des FPS standards (~60)
                dwarf.x += directionX * adjustedSpeed * elapsedTime;
                dwarf.y += directionY * adjustedSpeed * elapsedTime;

                // Augmente la fatigue et la faim en fonction du déplacement
                dwarf.fatigue += 2.0f * elapsedTime; // Ajuster les incréments
                dwarf.hunger += 1.5f * elapsedTime;
            } else {
                // Atteindre une nouvelle cible
                dwarf.targetX = GetRandomValue(1, MAP_WIDTH - 2);
                dwarf.targetY = GetRandomValue(1, MAP_HEIGHT - 2);
            }

            if (dwarf.hunger >= 50.0f) {
                bool foundFood = DwarfHuntPigSuccess(i);
                if (!foundFood) {
                    foundFood = DwarfHuntCowSuccess(i);
                }
            } else {
                if (dwarf.wood < 10) {
                    DwarfChopTree(i);
                } else {
                    DwarfMineRock(i);
                }
            }

            if (dwarf.fatigue >= 90.0f) {
                dwarf.resting = true;
                AddNotification(std::string(dwarf.name) + " est trop fatigué et se repose.");
            }

            dwarf.fatigue = Clamp(dwarf.fatigue, 0.0f, 100.0f);
            dwarf.hunger = Clamp(dwarf.hunger, 0.0f, 100.0f);
        }
    }
}





// Dessiner les nains
void DrawDwarfs()
{
    for (int i = 0; i < NUMBER_OF_DWARF; i++)
    {
        if (dwarfs[i].factionId == -1)
            continue; // Ne dessine que les nains avec une faction

        Color factionColor = factions[dwarfs[i].factionId].color;
        float dwarfX = (dwarfs[i].x * 16 + 8 + offsetX) * zoom;
        float dwarfY = (dwarfs[i].y * 16 + 8 + offsetY) * zoom;
        
        if (i == selectedDwarf)
        {
            DrawCircle(dwarfX, dwarfY, 10 * zoom, DARKGREEN);
        }

        // Dessine le nain
        DrawCircle(dwarfX, dwarfY, 8 * zoom, factionColor);
        DrawCircle(dwarfX, dwarfY, 6 * zoom, BLACK);

        //ajout de Dwarf name juste au dessus du nain
        DrawText(dwarfs[i].name, dwarfX - 20, dwarfY - 20, 10, BLACK);

        // Débogage visuel : cercle de détection en rouge
        //DrawCircleLines(dwarfX, dwarfY, 8 * zoom, GREEN);
    }
}

// Dessiner les informations sur le nain sélectionné
void DrawDwarfInfo() {
    if (selectedDwarf < 0 || selectedDwarf >= NUMBER_OF_DWARF) return;

    Dwarf &dwarf = dwarfs[selectedDwarf];
    
    // Position d'affichage
    int infoX = 10;
    int infoY = 550;

    // Affiche les informations du nain
    DrawText(TextFormat("Name: %s", dwarf.name), infoX, infoY, 20, BLACK);
    DrawText(TextFormat("Health: %d", dwarf.health), infoX, infoY + 20, 20, BLACK);

    // Vérifiez les valeurs avant l'affichage
    DrawText(TextFormat("Hunger: %.1f", dwarf.hunger), infoX, infoY + 40, 20, BLACK);
    DrawText(TextFormat("Fatigue: %.1f", dwarf.fatigue), infoX, infoY + 60, 20, BLACK);

    DrawText(TextFormat("Skill: %d", dwarf.skill), infoX, infoY + 80, 20, BLACK);
    DrawText(TextFormat("Resting: %s", dwarf.resting ? "Yes" : "No"), infoX, infoY + 100, 20, BLACK);

    // Si vous avez des informations de faction, assurez-vous qu'elles sont valides
    DrawText(TextFormat("Faction: %d", dwarf.factionId), infoX, infoY + 120, 20, BLACK);
}



// Sélectionner un nain
void SelectDwarf()
{
    Vector2 mousePos = GetMousePosition();

    // Ajuste la position de la souris pour tenir compte du zoom et des offsets
    float adjustedMouseX = (mousePos.x / zoom) - offsetX;
    float adjustedMouseY = (mousePos.y / zoom) - offsetY;

    for (int i = 0; i < NUMBER_OF_DWARF; i++)
    {
        // Calcul correct de la position du nain en coordonnées de la carte

        // dwarfX veut dire la position du nain en x
        float dwarfX = dwarfs[i].x * 16 + 8;
        // dwarfY veut dire la position du nain en y
        float dwarfY = dwarfs[i].y * 16 + 8;

        // Rayon de collision ajusté
        float radius = 8;

        // Vérifie si la position ajustée de la souris est dans le cercle du nain
        if (CheckCollisionPointCircle({adjustedMouseX, adjustedMouseY}, {dwarfX, dwarfY}, radius))
        {
            selectedDwarf = i;
            return; // Quitte la fonction une fois qu'un nain est sélectionné
        }
    }

    // Si aucun nain n'est sélectionné
    selectedDwarf = -1;
}
