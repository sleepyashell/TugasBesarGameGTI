#ifndef WORLD_H_INCLUDED 
#define WORLD_H_INCLUDED 

#include <vector>

const float FLOOR_HEIGHT = 5.0f;
const int NUM_FLOORS = 3;

struct BoundingBox {
    float xMin, xMax, zMin, zMax, yMin, yMax;
};

extern float playerX;
extern float playerY;
extern float playerZ;
extern float pRadius;
extern std::vector<BoundingBox> colliders;
extern bool isDoorOpen;
const int NUM_ROOMS_PER_FLOOR = 6;

// --- RUANGAN TERKUNCI ---
// lockedRooms[f][r] = true jika ruangan r di lantai f tidak bisa dimasuki
extern bool lockedRooms[NUM_FLOORS][NUM_ROOMS_PER_FLOOR];

// Generate ruangan terkunci secara random (panggil di init)
void randomizeLockedRooms();

// 4. Deklarasi Fungsi (Sesuaikan dengan parameter di World.cpp)
void setupLighting();
void updateLightPositions();
void registerCollider(float x, float z, float w, float d, float yMin = -1000.0f, float yMax = 1000.0f);
void handleStairs();
void buildPhysicalWorld();
bool checkCollision(float nextX, float nextZ);

#endif
