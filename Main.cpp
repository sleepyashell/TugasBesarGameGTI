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

using namespace std;

float playerX      = 33.0f;
float playerY      = 0.8f;
float playerZ      = 2.0f;
float playerSpeed  = 0.12f;
float pRadius      = 0.25f;
float playerAngle  = 0.0f;
float targetAngle  = 0.0f;
float walkTimer    = 0.0f;
bool  isWalking    = false;

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

void startIntroDialog() {
    if (introPlayed) return;
    
    cutsceneManager.addDialogLine("Kamu bangun di tempat yang gelap...", 3.0f, true);
    cutsceneManager.addDialogLine("Dimana aku? Ini... dimana?", 3.0f, true);
    cutsceneManager.addDialogLine("Ada seseorang di sini...", 3.0f, true);
    cutsceneManager.addDialogLine("Aku harus pergi dari sini!", 2.5f, false);
    
    cutsceneManager.startCutscene();
    introPlayed = true;
}

void display() {
    if (gameState == STATE_MENU) {
        drawMenu();
    } else if (gameState == STATE_PLAYING) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        setupCamera();
        handleAllLighting();

        drawGround();
        //// drawTrees();
        drawItems();
        drawBot();
        
        
        drawPlayer();
        drawRuangGedung();
        drawAllPosters();
        
        // HUD DEBUG: KOORDINAT PLAYER
        char coordsStr[64];
        sprintf(coordsStr, "PLAYER POS -> X: %.2f  Y: %.2f  Z: %.2f", playerX, playerY, playerZ);
        
        drawHUDText(2.0f, 95.0f, coordsStr);
        int currentFloor = (int)(playerY / 4.0f) + 1; 
        char floorStr[32];
        sprintf(floorStr, "LANTAI: %d", currentFloor);
        drawHUDText(2.0f, 91.0f, floorStr);
        drawItemHUD();
        
        // Render cutscene dialog if active
        if (cutsceneManager.isRunning()) {
            cutsceneManager.render();
        }
        
        glutSwapBuffers();
    }
}


void update(int v) {
    handleAllInput();
    updateItems(0.016f);
    checkItemPickup();
    bool animChanged = updateDoorAnimations(0.016f);
    checkDoorProximity();
    buildPhysicalWorld();
    
    // Update cutscene
    cutsceneManager.update(0.016f);
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
    updateBot();
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
    
    // Initialize sound manager
    soundManager.initialize();
    
    // Random door state saat game start
    isDoorOpen = (rand() % 2) == 0;
    
    glutIgnoreKeyRepeat(1);
    
    // --- RUANGAN TERKUNCI ---
    // Generate ruangan yang tidak bisa dimasuki secara random
    randomizeLockedRooms();
    buildPhysicalWorld();
    initItems();
    initBot();
    
    // Inisialisasi menu
    initMenu();
    
    // Play background music (commented to avoid startup issues)
    // soundManager.playSound(SOUND_BACKGROUND);
}


void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55.0, (float)w / h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    if (gameState == STATE_MENU) {
        handleMenuInput(key);
    } else if (gameState == STATE_PLAYING) {
        pressNormalKeys(key, x, y);
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    if (gameState == STATE_PLAYING) {
        releaseNormalKeys(key, x, y);
    }
}

void specialKeys(int key, int x, int y) {
    if (gameState == STATE_MENU) {
        if (key == GLUT_KEY_UP) {
            selectedMenuItem = 0;  // PLAY
        } else if (key == GLUT_KEY_DOWN) {
            selectedMenuItem = 1;  // EXIT
        }
    }
}

void mouse(int button, int state, int x, int y) {
    handleMouseClick(button, state, x, y);
}

void cleanup() {
    cleanupMenuTextures();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("HORROR GAME - GTI");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);   
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);
    
    // Cleanup on exit
    atexit(cleanup);
    
    glutMainLoop();
    return 0;
}
