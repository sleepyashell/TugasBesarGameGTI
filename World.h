#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef WORLD_H
#define WORLD_H

#include "Player.h"
#include <vector>

const float FLOOR_HEIGHT = 5.0f;
const int NUM_FLOORS = 3;
const int NUM_ROOMS_PER_FLOOR = 6;
extern bool lockedRooms[NUM_FLOORS][NUM_ROOMS_PER_FLOOR];

struct BoundingBox {
    float xMin, xMax, zMin, zMax, yMin, yMax;
};

struct DoorAnimation {
    bool isOpen;           
    bool isAnimating;      
    float currentAngle;    
    float targetAngle;     
    float animSpeed;       
    int roomIndex;         
    int floor;             
};

extern DoorAnimation doorAnims[NUM_FLOORS][NUM_ROOMS_PER_FLOOR];

const float DOOR_OPEN_DISTANCE = 3.5f;   
const float DOOR_CLOSE_DISTANCE = 5.5f;  
void initDoorAnimations();
bool updateDoorAnimations(float dt);
void triggerDoorOpen(int floor, int roomIndex);
void triggerDoorClose(int floor, int roomIndex);
void checkDoorProximity();
struct DoorInfo {
    float x;        
    float z;        
    float w;        
    float d;        
    bool flipped;   
    float roomStartX; 
    float roomWidth;  
};
bool isAtGate();
extern DoorInfo doorInfos[NUM_ROOMS_PER_FLOOR];
int getRoomIndexFromX(float x);
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
extern bool lockedRooms[NUM_FLOORS][NUM_ROOMS_PER_FLOOR];

#endif
