#ifndef WORLD_H_INCLUDED 
#define WORLD_H_INCLUDED 

#include <vector>

const float FLOOR_HEIGHT = 5.0f;
const int NUM_FLOORS = 3;
const int NUM_ROOMS_PER_FLOOR = 6;
extern bool lockedRooms[NUM_FLOORS][NUM_ROOMS_PER_FLOOR];

struct BoundingBox {
    float xMin, xMax, zMin, zMax, yMin, yMax;
};

// === ANIMASI PINTU ===
struct DoorAnimation {
    bool isOpen;           
    bool isAnimating;      
    float currentAngle;    // 0 = tertutup, 90 = terbuka ke DALAM
    float targetAngle;     
    float animSpeed;       
    int roomIndex;         
    int floor;             
};

extern DoorAnimation doorAnims[NUM_FLOORS][NUM_ROOMS_PER_FLOOR];

// Jarak trigger pintu terbuka/tutup
const float DOOR_OPEN_DISTANCE = 3.5f;   // Jarak buka
const float DOOR_CLOSE_DISTANCE = 5.5f;  // Jarak tutup

void initDoorAnimations();
bool updateDoorAnimations(float dt);
void triggerDoorOpen(int floor, int roomIndex);
void triggerDoorClose(int floor, int roomIndex);
void checkDoorProximity();

// === POSISI PINTU GLOBAL (ABSOLUT) ===
struct DoorInfo {
    float x;        // Posisi X absolut engsel kiri pintu
    float z;        // Posisi Z pintu (biasanya 0)
    float w;        // Lebar total pintu (2.0 untuk double door)
    float d;        // Kedalaman (0.2)
    bool flipped;   // true jika engsel di kanan
    float roomStartX; // Start X ruangan ini
    float roomWidth;  // Lebar ruangan (8.0)
};

// Posisi pintu absolut per ruangan
extern DoorInfo doorInfos[NUM_ROOMS_PER_FLOOR];

// Helper: dapatkan roomIndex dari posisi X player
int getRoomIndexFromX(float x);

// Helper: cek apakah player di depan pintu
bool isPlayerInFrontOfDoor(int floor, int roomIndex);

extern float playerX;
extern float playerY;
extern float playerZ;
extern float pRadius;
extern std::vector<BoundingBox> colliders;

void setupLighting();
void updateLightPositions();
void registerCollider(float x, float z, float w, float d, float yMin = -1000.0f, float yMax = 1000.0f);
void handleStairs();
void buildPhysicalWorld();
bool checkCollision(float nextX, float nextZ);
void randomizeLockedRooms();

#endif
