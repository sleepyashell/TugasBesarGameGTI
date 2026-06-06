#include "Camera.h"

#include <cmath>

#include "Player.h"

void setupCamera(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(playerX, playerY + 1.2f, playerZ + 4.0f,
              playerX, playerY + 0.5f, playerZ,
              0.0f   , 1.0f          , 0.0f);
}
