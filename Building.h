#ifndef BUILDING_H_INCLUDED 
#define BUILDING_H_INCLUDED 
// ==========================================
// MODULE BANGUNAN
// ==========================================

void drawCorridorInterior(float width, float depth);

void drawCorridorFront(float width, float floorY);

void drawRoomInterior(float width, float depth);

void drawStairArea(float width, float depth, bool isLastFloor);

void drawStairDown();
void drawDoubleDoor(float alpha);

void drawFrontWall(float width, float depth, float startX, bool isFlipped,
                   float floorY);
void drawOneLantai(float offsetY, bool isLastFloor, bool hasStairDown);
void drawRuangGedung();

#endif
