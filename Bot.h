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
    float dirX;
    int currentFloor;
    int targetNodeIndex; 
    bool movingRight;
    bool isChasing;
    bool isSearching;
};

extern EnemyBot ghostBot;

// Fungsi Utama
void initBot();
void updateBot();
void drawBot();

// Fungsi Modularisasi State AI (Helper)
void handleStairInterpolation();
bool scanForPlayer();
void executeChaseMode(int playerFloor);
void executeStairPatrol();
void executeCorridorPatrol(int baseIndex);
void checkStairTrigger(int playerFloor, int baseIndex);

#endif
