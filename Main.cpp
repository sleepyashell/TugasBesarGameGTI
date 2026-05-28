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
#include "Player.h"
#include "Bot.h"

using namespace std;

bool keys[256];
float playerX      = 33.0f;
float playerY      = 0.8f;
float playerZ      = 2.0f;
float playerSpeed  = 0.12f;
float pRadius      = 0.25f;
float playerAngle  = 0.0f;
float targetAngle  = 0.0f;
float walkTimer    = 0.0f;
bool  isWalking    = false;

float flickerTimer     = 0.0f;
float flickerIntensity = 1.0f;

vector<BoundingBox> colliders;

#include <cstdio> // Pastikan header ini ada untuk fungsi sprintf

// Fungsi untuk menggambar teks 2D di layar
void drawHUDText(float x, float y, const char* text) {
    // 1. Matikan pencahayaan sementara agar warna teks solid (putih/hijau)
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // 2. Pindah ke mode Proyeksi Matriks Sementara untuk merender teks 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    // Set koordinat layar 2D (0 sampai 100 secara horizontal dan vertikal)
    gluOrtho2D(0, 100, 0, 100);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // 3. Set warna teks (Hijau terang/Lime agar kontras di kegelapan game horor)
    glColor3f(0.0f, 1.0f, 0.0f); 
    
    // 4. Tentukan posisi teks (x, y) di layar berdasarkan skala Ortho 0-100
    glRasterPos2f(x, y);
    
    // 5. Gambar karakter teks satu per satu
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
    
    // 6. Kembalikan kondisi matriks OpenGL ke semula (3D Mode)
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    // Hidupkan kembali sistem lighting gedung
    glEnable(GL_LIGHTING);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(playerX, playerY + 1.2f, playerZ + 12.0f,
              playerX, playerY + 0.5f, playerZ,
              0.0f, 1.0f, 0.0f);

    flickerTimer += 0.05f;
    flickerIntensity = 0.75f
        + 0.15f * sinf(flickerTimer * 7.3f)
        + 0.08f * sinf(flickerTimer * 23.1f)
        + 0.04f * sinf(flickerTimer * 57.9f);
    if (flickerIntensity < 0.0f) flickerIntensity = 0.0f;

    updateLightPositions();

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
    drawBot();

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

    drawPlayer();
    
    // ==========================================
    // HUD DEBUG: TAMPILKAN KOORDINAT PLAYER
    // ==========================================
    char coordsStr[64];
    // Ambil data X, Y, Z player dan format menjadi teks string
    sprintf(coordsStr, "PLAYER POS -> X: %.2f  Y: %.2f  Z: %.2f", playerX, playerY, playerZ);
    
    // Gambar teks di koordinat layar (X=2, Y=95) -> Pojok kiri atas layar
    drawHUDText(2.0f, 95.0f, coordsStr);

    // Tambahan info lantai biar makin gampang mantau bot
    int currentFloor = (int)(playerY / 4.0f) + 1; // 4.0f adalah FLOOR_HEIGHT kamu
    char floorStr[32];
    sprintf(floorStr, "LANTAI: %d", currentFloor);
    drawHUDText(2.0f, 91.0f, floorStr);
    // ==========================================

    glutSwapBuffers();
    
    
}

void handleInput() {
    float mx = 0, mz = 0;
    if (keys['w'] || keys['W']) mz -= 1;
    if (keys['s'] || keys['S']) mz += 1;
    if (keys['a'] || keys['A']) mx -= 1;
    if (keys['d'] || keys['D']) mx += 1;
    if (abs(mx) + abs(mz) > 0) {
        isWalking = true;
        walkTimer += 0.15f;
        targetAngle = atan2f(mx, mz) * 180.0f / 3.14159f;
        float mag = sqrt(mx * mx + mz * mz);
        float sx  = (mx / mag) * playerSpeed;
        float sz  = (mz / mag) * playerSpeed;
        if (!checkCollision(playerX + sx, playerZ)) playerX += sx;
        if (!checkCollision(playerX, playerZ + sz)) playerZ += sz;
    } else {
        isWalking = false;
    }

    float diff = targetAngle - playerAngle;
    if (diff > 180.0f)  diff -= 360.0f;
    if (diff < -180.0f) diff += 360.0f;
    playerAngle += diff * 0.15f;

    handleStairs();
}

void update(int v) {
    handleInput();
    updateItems(0.016f);
    checkItemPickup();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
    updateBot();
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
    initBot();
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
