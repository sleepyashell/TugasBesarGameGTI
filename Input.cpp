#include "Input.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "World.h"
#include "Cutscene.h"
#include "Menu.h"
#include "Player.h"
#include "Sound.h"
#include "Jumpscare.h"

extern int selectedMenuItem;
extern void restartGame();
extern void returnToMenu();

int deltaMoveX = 0;
int deltaMoveZ = 0;

void pressNormalKeys(unsigned char key, int x, int y) {
    if (gameState == STATE_WIN) {
        if (key == 13) {  
            if (selectedMenuItem == 0) {
                
                returnToMenu();
            } else {
                cleanupMenuTextures();
                exit(0);
            }
        }
        return;
    }

    if (jumpscareManager.isGameOver()) {
        jumpscareManager.handleInput(key);
        return;
    }
    if (key == 13) {  
        if (cutsceneManager.isRunning()) {
            cutsceneManager.advanceDialog();
            return;
        }
    }

    if (gameState == STATE_MENU) {
        switch (key) {
            case 'w': case 'W': selectedMenuItem = 0; break;
            case 's': case 'S': selectedMenuItem = 1; break;
            case 13:  
                if (selectedMenuItem == 0) {
                    gameState = STATE_PLAYING;
                    startIntroDialog();
                    soundManager.playSound(SOUND_BELL);
                } else if (selectedMenuItem == 1) {
                    cleanupMenuTextures();
                    exit(0);
                }
                break;
            case 27:  
                cleanupMenuTextures();
                exit(0);
                break;
        }
        return;
    }

    if (gameState == STATE_PLAYING) {
        switch (key) {
            case 'w': case 'W': deltaMoveZ = -1; break;
            case 's': case 'S': deltaMoveZ =  1; break;
            case 'a': case 'A': deltaMoveX = -1; break;
            case 'd': case 'D': deltaMoveX =  1; break;
            case 27:  
                exit(0);
                break;
        }
    }
}

void releaseNormalKeys(unsigned char key, int x, int y) {
    if (cutsceneManager.isRunning()) {
        return;
    }
    if (gameState == STATE_PLAYING) {
        switch (key) {
            case 'w': case 'W': if (deltaMoveZ < 0) deltaMoveZ = 0; break;
            case 's': case 'S': if (deltaMoveZ > 0) deltaMoveZ = 0; break;
            case 'a': case 'A': if (deltaMoveX < 0) deltaMoveX = 0; break;
            case 'd': case 'D': if (deltaMoveX > 0) deltaMoveX = 0; break;
        }
    }
}

void specialKeys(int key, int x, int y) {
    if (gameState == STATE_WIN) {
        if (key == GLUT_KEY_UP)   selectedMenuItem = 0;  
        if (key == GLUT_KEY_DOWN) selectedMenuItem = 1;  
        glutPostRedisplay();  
        return;
    }

    if (jumpscareManager.isGameOver()) {
        jumpscareManager.handleInput(key);
        return;
    }

    if (gameState == STATE_MENU) {
        if (key == GLUT_KEY_UP) {
            selectedMenuItem = 0;  
        } else if (key == GLUT_KEY_DOWN) {
            selectedMenuItem = 1;  
        }
    }
}

void mouse(int button, int state, int x, int y) {
    if (gameState != STATE_MENU) return;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (y >= 250 && y <= 380 && x >= 450 && x <= 830) {
            gameState = STATE_PLAYING;
            startIntroDialog();
        }
        else if (y >= 380 && y <= 510 && x >= 450 && x <= 830) {
            cleanupMenuTextures();
            exit(0);
        }
    }
}

void inputMovement() {
    if (cutsceneManager.isRunning()) {
        isWalking = false;
        return;
    }

    if (deltaMoveX != 0 || deltaMoveZ != 0) {
        isWalking = true;
        walkTimer += 0.15f;
        targetAngle = atan2f((float)deltaMoveX, (float)deltaMoveZ) * 180.0f / 3.14159f;

        float mx = (float)deltaMoveX;
        float mz = (float)deltaMoveZ;
        float mag = sqrt(mx * mx + mz * mz);

        float sx = (mx / mag) * playerSpeed;
        float sz = (mz / mag) * playerSpeed;

        float origX = playerX;
        float origZ = playerZ;

        if (!checkCollision(origX + sx, origZ)) playerX = origX + sx;
        if (!checkCollision(origX, origZ + sz)) playerZ = origZ + sz;
    } else {
        isWalking = false;
    }

    float diff = targetAngle - playerAngle;
    if (diff > 180.0f)  diff -= 360.0f;
    if (diff < -180.0f) diff += 360.0f;
    playerAngle += diff * 0.15f;

    handleStairs();
}
