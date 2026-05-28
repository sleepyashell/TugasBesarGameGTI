#ifndef WORLD_H_INCLUDED 
#define WORLD_H_INCLUDED 

#include <vector>

// 1. Pindahkan Konstanta ke sini agar bisa dibaca semua file
const float FLOOR_HEIGHT = 5.0f;
const int NUM_FLOORS = 3;

// 2. Pindahkan Struct BoundingBox ke sini
struct BoundingBox {
    float xMin, xMax, zMin, zMax, yMin, yMax;
};

// 3. Deklarasikan 'extern' supaya World.cpp tahu variabel ini ada di Main.cpp
extern float playerX;
extern float playerY;
extern float playerZ;
extern float pRadius;
extern std::vector<BoundingBox> colliders;

// 4. Deklarasi Fungsi (Sesuaikan dengan parameter di World.cpp)
void setupLighting();
void updateLightPositions();
void registerCollider(float x, float z, float w, float d, float yMin = -1000.0f, float yMax = 1000.0f);
void handleStairs();
void buildPhysicalWorld();
bool checkCollision(float nextX, float nextZ);

#endif
