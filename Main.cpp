#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>	
#include <cstdio> 

#include "Material.h"
#include "World.h"
#include "Building.h"
#include "Drawing.h"
#include "Texture.h"
#include "Item.h"
#include "Player.h"
#include "Bot.h"
#include "Input.h"
#include "Camera.h"
#include "Lighting.h"
#include "Menu.h"
#include "Sound.h"
#include "Cutscene.h"
#include "Jumpscare.h"
#include "Shadow.h"

using namespace std;

vector<BoundingBox> colliders;

bool isDoorOpen = false;
bool introPlayed = false;


void drawHUDText(float x, float y, const char* text) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor3f(0.0f, 1.0f, 0.0f); 
    
    glRasterPos2f(x, y);
    
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_LIGHTING);
}

void restartGame(){
	soundManager.stopAllSounds();
    playerX = -4.0f;
    playerY = 0.8f;
    playerZ = 2.0f;
    playerAngle = 0.0f;
    targetAngle = 0.0f;
    isWalking = false;
    playerWon = false;

    initDoorAnimations();
    randomizeLockedRooms();
    buildPhysicalWorld();

    resetItems();
    initBot();
    jumpscareManager.reset();
    introPlayed = true;
    startIntroDialog();
    soundManager.playSound(SOUND_BACKGROUND);
}

void returnToMenu(){
    
    extern int deltaMoveX;
    extern int deltaMoveZ;
    playerWon = false;
    introPlayed = false;
    deltaMoveX = 0;
    deltaMoveZ = 0;
    jumpscareManager.reset();
    cutsceneManager.stopCutscene();
    gameState = STATE_MENU;
    selectedMenuItem = 0;
    glutPostRedisplay();
}


void startIntroDialog() {
    if (introPlayed) return;
    
    cutsceneManager.addDialogLine("Aduh kepala ku sakit...", 3.0f, true);
    cutsceneManager.addDialogLine("Dimana aku? Ini... dimana?", 3.0f, true);
    cutsceneManager.addDialogLine("Ada seseorang di sini...", 3.0f, true);
    cutsceneManager.addDialogLine("Aku harus pergi dari sini!", 2.5f, false);
    
    cutsceneManager.startCutscene();
    introPlayed = true;
}

void drawCoordinateHUD() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    char buffer[128];
    
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
        glVertex2f(0, 92);
        glVertex2f(30, 92);
        glVertex2f(30, 100);
        glVertex2f(0, 100);
    glEnd();
    glDisable(GL_BLEND);
    
    
    glColor3f(0.0f, 1.0f, 0.0f);
    snprintf(buffer, sizeof(buffer), "X:%.2f | Y:%.2f | Z:%.2f", playerX, playerY, playerZ);
    glRasterPos2f(1, 96);
    for (size_t i = 0; i < strlen(buffer); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buffer[i]);
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void display() {
    if (gameState == STATE_MENU) {
        drawMenu();
    } else if (gameState == STATE_WIN) {
        drawWinScreen();
    } else if (gameState == STATE_PLAYING) {
        if (jumpscareManager.isActive()) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            jumpscareManager.render();
            glutSwapBuffers();
            return;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        setupCamera();
        handleAllLighting();

        
        drawGround();
        drawTrees();
        drawGate();
        drawOuterWalls();
        drawRuangGedung();   
        drawItems();
        drawBot();

        
        int playerFloor = (int)(playerY / FLOOR_HEIGHT);
        float shadowY = playerFloor * FLOOR_HEIGHT + 0.06f;
        updateShadowMatrix(shadowY);
        beginShadow();
        drawPlayer();        
        endShadow();

        
        drawPlayer();

        drawItemHUD();

        
        if (cutsceneManager.isRunning()) {
            cutsceneManager.render();
        }
        
        glutSwapBuffers();
    }
}


void update(int v) {
    
    if (gameState != STATE_WIN) {
        jumpscareManager.update(0.016f);
    }

    if (jumpscareManager.isActive() && gameState != STATE_WIN) {
        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
        return;
    }

    inputMovement();
    updateItems(0.016f);
    checkItemPickup();
    if (!playerWon && allRequiredItemsCollected() && isAtGate()){
        playerWon = true;

        cutsceneManager.addDialogLine(
            "Aku berhasil keluar dari tempat ini...",
            4.0f,
            false
        );

        cutsceneManager.addDialogLine(
            "Aku selamat.",
            3.0f,
            false
        );

            cutsceneManager.startCutscene();
    }
    
    updateBot();
    bool animChanged = updateDoorAnimations(0.016f);
    checkDoorProximity();
    cutsceneManager.update(0.016f);

    
    if (playerWon && !cutsceneManager.isRunning() && gameState != STATE_WIN) {
        gameState = STATE_WIN;
        selectedMenuItem = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.04f, 0.04f, 0.10f, 1.0f);
    initDoorAnimations();
    setupLighting();
    initTextures();
    initPosters();
    srand(time(0));
    
    soundManager.initialize();
    jumpscareManager.loadTextures();
    isDoorOpen = (rand() % 2) == 0;
    
    glutIgnoreKeyRepeat(1);
    
    randomizeLockedRooms();
    buildPhysicalWorld();
    initItems();
    initBot();
    initMenu();
}


void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55.0, (float)w / h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

void cleanup() {
    cleanupMenuTextures();
    jumpscareManager.cleanupTextures();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("HORROR GAME - GTI");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(pressNormalKeys);   
    glutKeyboardUpFunc(releaseNormalKeys);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);
    
    
    atexit(cleanup);
    
    glutMainLoop();
    return 0;
}
