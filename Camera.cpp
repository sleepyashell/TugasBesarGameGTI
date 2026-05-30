#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include "Camera.h"

// Ambil koordinat dan sudut pandang player dari Main.cpp
extern float playerX;
extern float playerY;
extern float playerZ;
extern float playerAngle;

void setupCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(playerX, playerY + 1.2f, playerZ + 4.0f,
              playerX, playerY + 0.5f, playerZ,
              0.0f, 1.0f, 0.0f);
}
