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
    int targetNodeIndex; // Mengunci indeks node tujuan aktual
    bool movingRight;
    bool isChasing;
    bool isSearching;
};

extern EnemyBot ghostBot;

void initBot();
void updateBot();
void drawBot();

#endif
