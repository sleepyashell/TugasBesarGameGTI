#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <vector>
#include <cstdlib>

#include "Material.h"
#include "World.h"
#include "Building.h"
#include "Drawing.h"
#include "Texture.h"
#include "Item.h"

using namespace std;
// ==========================================
// DATA STRUCTURE & GLOBAL STATE
// ==========================================

bool keys[256];
float playerX      = 33.0f;
float playerY      = 0.8f;
float playerZ      = 2.0f;
float playerSpeed  = 0.12f;
float pRadius      = 0.25f;


// TAMBAHKAN DUA BARIS INI DI SINI:
float flickerTimer = 0.0f;
float flickerIntensity = 1.0f;

vector<BoundingBox> colliders;

// ==========================================
// CORE
// ==========================================

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(playerX, playerY + 1.2f, playerZ + 12.0f,
              playerX, playerY + 0.5f, playerZ,
              0.0f, 1.0f, 0.0f);

    // Update flicker lampu darurat
    flickerTimer += 0.05f;
    // Kombinasi beberapa sinus frekuensi berbeda ? efek flicker tidak beraturan
    flickerIntensity = 0.75f
        + 0.15f * sinf(flickerTimer * 7.3f)
        + 0.08f * sinf(flickerTimer * 23.1f)
        + 0.04f * sinf(flickerTimer * 57.9f);
    if (flickerIntensity < 0.0f) flickerIntensity = 0.0f;

    // Update posisi light setiap frame 
    updateLightPositions();

    // Terapkan flicker ke lampu koridor
    GLenum ptLights[] = { GL_LIGHT2, GL_LIGHT3, GL_LIGHT4 };
    for (int f = 0; f < NUM_FLOORS; f++) {
        GLfloat pt_diff[] = {
            0.75f * flickerIntensity,
            0.68f * flickerIntensity,
            0.35f * flickerIntensity, 1.0f
        };
        glLightfv(ptLights[f], GL_DIFFUSE, pt_diff);
    }

    drawGround();
    drawTrees();
    drawRuangGedung();
    drawAllPosters();
    drawItems();

    // Elemen transparan per lantai (front walls & corridor front)
    for (int f = 0; f < NUM_FLOORS; f++) {
        float fy = f * FLOOR_HEIGHT;
        glPushMatrix();
            glTranslatef(0, fy, 4);
            drawCorridorFront(56, fy);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, fy, 0);
            drawFrontWall(8, 10,  0, false, fy); glTranslatef(8, 0, 0);
            drawFrontWall(8, 10,  8, true,  fy); glTranslatef(8, 0, 0);
            drawFrontWall(8, 10, 16, false, fy); glTranslatef(8, 0, 0);
            drawFrontWall(8, 10, 24, true,  fy); glTranslatef(16, 0, 0);
            drawFrontWall(8, 10, 40, false, fy); glTranslatef(8, 0, 0);
            drawFrontWall(8, 10, 48, false, fy);
        glPopMatrix();
    }

    // Player 
    glPushMatrix();
        glTranslatef(playerX, playerY, playerZ);
        matPlayer();
        glScalef(0.6f, 1.6f, 0.3f);
        glutSolidCube(1);
    glPopMatrix();

    glutSwapBuffers();
}

void handleInput() {
    float mx = 0, mz = 0;
    if (keys['w'] || keys['W']) mz -= 1;
    if (keys['s'] || keys['S']) mz += 1;
    if (keys['a'] || keys['A']) mx -= 1;
    if (keys['d'] || keys['D']) mx += 1;
    if (abs(mx) + abs(mz) > 0) {
        float mag = sqrt(mx * mx + mz * mz);
        float sx  = (mx / mag) * playerSpeed;
        float sz  = (mz / mag) * playerSpeed;
        if (!checkCollision(playerX + sx, playerZ)) playerX += sx;
        if (!checkCollision(playerX, playerZ + sz)) playerZ += sz;
    }
    handleStairs();
}

void update(int v) {
    handleInput();
    updateItems(0.016f);
    checkItemPickup();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);

}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.04f, 0.04f, 0.10f, 1.0f); 


    setupLighting();
    initPosters();
    buildPhysicalWorld();
    initItems();
}

void keyPressed(unsigned char k, int x, int y) { keys[k] = true; }
void keyUp(unsigned char k, int x, int y)      { keys[k] = false; }

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55.0, (float)w / h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("HORROR GAME - GTI");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyUp);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
