#include "Jumpscare.h"

#include <cstdio>
#include <cstdlib>

#include "Texture.h" 
#include "Sound.h"

JumpscareManager jumpscareManager;

JumpscareManager::JumpscareManager() 
    : state(JUMPSCARE_NONE), timer(0.0f), jumpscareDuration(2.5f), 
      selectedOption(0), jumpscareTex(0), gameoverTex(0), gameoverExitTex(0), 
      texturesLoaded(false) {}

JumpscareManager::~JumpscareManager() {
    cleanupTextures();
}

void JumpscareManager::loadTextures() {
    if (texturesLoaded) return;
    printf("[Jumpscare] Loading textures from Assets/ folder...\n");

    jumpscareTex = loadBMP("Assets/jumpscare.bmp");
    if (jumpscareTex == 0) jumpscareTex = loadBMP("jumpscare.bmp");
    if (jumpscareTex == 0) jumpscareTex = loadBMP("../Assets/jumpscare.bmp");

    gameoverTex = loadBMP("Assets/gameoverrestarthighlight.bmp");
    if (gameoverTex == 0) gameoverTex = loadBMP("gameoverrestarthighlight.bmp");
    if (gameoverTex == 0) gameoverTex = loadBMP("../Assets/gameoverrestarthighlight.bmp");

    gameoverExitTex = loadBMP("Assets/gameoverexithighlight.bmp");
    if (gameoverExitTex == 0) gameoverExitTex = loadBMP("gameoverexithighlight.bmp");
    if (gameoverExitTex == 0) gameoverExitTex = loadBMP("../Assets/gameoverexithighlight.bmp");

    texturesLoaded = true;
    printf("[Jumpscare] Textures loaded: jumpscare=%u, gameoverRestart=%u, gameoverExit=%u\n",
           jumpscareTex, gameoverTex, gameoverExitTex);

    if (jumpscareTex == 0)
        printf("[Jumpscare] ERROR: Assets/jumpscare.bmp failed to load!\n");
    if (gameoverTex == 0)
        printf("[Jumpscare] ERROR: Assets/gameoverrestarthighlight.bmp failed to load!\n");
    if (gameoverExitTex == 0)
        printf("[Jumpscare] ERROR: Assets/gameoverexithighlight.bmp failed to load!\n");
}

void JumpscareManager::cleanupTextures() {
    if (jumpscareTex)    glDeleteTextures(1, &jumpscareTex);
    if (gameoverTex)     glDeleteTextures(1, &gameoverTex);
    if (gameoverExitTex) glDeleteTextures(1, &gameoverExitTex);
    jumpscareTex = gameoverTex = gameoverExitTex = 0;
    texturesLoaded = false;
}

void JumpscareManager::startJumpscare() {
    state = JUMPSCARE_PLAYING;
    timer = 0.0f;
    selectedOption = 0;

    soundManager.stopSound(SOUND_BACKGROUND);
    soundManager.stopSound(SOUND_CHASE);
    soundManager.playSound(SOUND_JUMPSCARE);
    glutPostRedisplay();
    printf("[Jumpscare] Started!\n");
}

void JumpscareManager::update(float dt) {
    if (state == JUMPSCARE_NONE) return;

    if (state == JUMPSCARE_PLAYING) {
        timer += dt;

        if (timer >= jumpscareDuration) {
            state = JUMPSCARE_GAMEOVER;
            timer = 0.0f;
            printf("[Jumpscare] Switching to GAMEOVER\n");
        }
    }
}

void JumpscareManager::render() {
    if (state == JUMPSCARE_NONE) return;

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1280, 0, 720);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    if (state == JUMPSCARE_PLAYING) {
        if (jumpscareTex) {
            glBindTexture(GL_TEXTURE_2D, jumpscareTex);
            glColor3f(1.0f, 1.0f, 1.0f);

			glBegin(GL_QUADS);
			    glTexCoord2f(1, 0); glVertex2f(0,    0);
			    glTexCoord2f(0, 0); glVertex2f(1280, 0);
			    glTexCoord2f(0, 1); glVertex2f(1280, 720);
			    glTexCoord2f(1, 1); glVertex2f(0,    720);
			glEnd();
			        } else {

            glDisable(GL_TEXTURE_2D);
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_QUADS);
                glVertex2f(0,    0);
                glVertex2f(1280, 0);
                glVertex2f(1280, 720);
                glVertex2f(0,    720);
            glEnd();
            glEnable(GL_TEXTURE_2D);
        }
    } else if (state == JUMPSCARE_GAMEOVER) {
        GLuint activeTex = (selectedOption == 0) ? gameoverTex : gameoverExitTex;

        if (activeTex) {
            glBindTexture(GL_TEXTURE_2D, activeTex);
            glColor3f(1.0f, 1.0f, 1.0f);

            glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex2f(0,    0);
                glTexCoord2f(1, 0); glVertex2f(1280, 0);
                glTexCoord2f(1, 1); glVertex2f(1280, 720);
                glTexCoord2f(0, 1); glVertex2f(0,    720);
            glEnd();
        } else {
            glDisable(GL_TEXTURE_2D);
            glColor3f(0.0f, 0.0f, 1.0f);
            glBegin(GL_QUADS);
                glVertex2f(0,    0);
                glVertex2f(1280, 0);
                glVertex2f(1280, 720);
                glVertex2f(0,    720);
            glEnd();
            glEnable(GL_TEXTURE_2D);
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void JumpscareManager::handleInput(int key) {
    if (state != JUMPSCARE_GAMEOVER) return;
    switch (key) {
        case GLUT_KEY_UP:
            selectedOption = 0;  
            printf("[Jumpscare] Selected: Restart\n");
            break;
        case GLUT_KEY_DOWN:
            selectedOption = 1;  
            printf("[Jumpscare] Selected: Exit\n");
            break;
        case 13:  
            if (selectedOption == 0) {
                printf("[Jumpscare] Restarting game...\n");
                reset();
                restartGame();
            } else {
                printf("[Jumpscare] Exiting game...\n");
                exit(0);
            }
            break;
    }
}

bool JumpscareManager::isActive() const {
    return state != JUMPSCARE_NONE;
}

bool JumpscareManager::isGameOver() const {
    return state == JUMPSCARE_GAMEOVER;
}

void JumpscareManager::reset() {
    state = JUMPSCARE_NONE;
    timer = 0.0f;
    selectedOption = 0;
    soundManager.stopSound(SOUND_JUMPSCARE);
}
