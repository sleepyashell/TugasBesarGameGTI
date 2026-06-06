#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef BOT_H
#define BOT_H

extern float playerX;
extern float playerY;
extern float playerZ;
extern bool checkCollision(float nextX, float nextZ);

struct Waypoint {
    float x, y, z;
};

struct EnemyBot {
    float x;
    float y;
    float z;
    float speed;
    float visionX;
    int currentFloor;
    int targetNodeIndex; 
    bool movingRight;
    bool isChasing;
    bool isSearching;
};

extern EnemyBot ghostBot[3];


void initBot();
void updateBot();
void drawBot();


void handleStairInterpolation();
bool scanForPlayer();
void executeChaseMode(int playerFloor);
void executeStairPatrol();
void executeCorridorPatrol(int baseIndex);
void checkStairTrigger(int playerFloor, int baseIndex);

#endif
