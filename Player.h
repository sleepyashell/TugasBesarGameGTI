#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef PLAYER_H
#define PLAYER_H

extern float playerX;
extern float playerY;
extern float playerZ;
extern float playerSpeed;
extern float pRadius;
extern float playerAngle;
extern float targetAngle;
extern float walkTimer;
extern bool  isWalking;
extern bool  playerWon;

void drawPlayer();

#endif
