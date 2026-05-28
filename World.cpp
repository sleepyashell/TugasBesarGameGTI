#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <vector>
#include <cstdlib>

#include "World.h"
#include "Building.h"
#include "Drawing.h"
// ==========================================
// SETUP LIGHTING
// ==========================================

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // --- Cahaya 0: Bulan  ---
    GLfloat moon_pos[]  = { 0.4f, 1.0f, 0.3f, 0.0f };
    GLfloat moon_diff[] = { 0.28f, 0.32f, 0.42f, 1.0f }; 
    GLfloat moon_spec[] = { 0.18f, 0.20f, 0.28f, 1.0f };
    GLfloat moon_amb[]  = { 0.0f,  0.0f,  0.0f,  1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, moon_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  moon_diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, moon_spec);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  moon_amb);
    glEnable(GL_LIGHT0);

    // --- Cahaya 1: Pantulan Ground  ---
    GLfloat gnd_pos[]  = { 0.0f, -1.0f, 0.0f, 0.0f };
    GLfloat gnd_diff[] = { 0.12f, 0.05f, 0.04f, 1.0f };
    GLfloat gnd_spec[] = { 0.0f,  0.0f,  0.0f,  1.0f };
    GLfloat gnd_amb[]  = { 0.0f,  0.0f,  0.0f,  1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, gnd_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  gnd_diff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, gnd_spec);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  gnd_amb);
    glEnable(GL_LIGHT1);

    // --- Global ambient ---
    GLfloat global_amb[] = { 0.10f, 0.10f, 0.13f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_amb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // --- Cahaya 2 ... : Lampu flickering di koridor ---
    GLenum ptLights[] = { GL_LIGHT2, GL_LIGHT3, GL_LIGHT4 };
    for (int f = 0; f < NUM_FLOORS; f++) {
        float fy = f * FLOOR_HEIGHT + 3.5f;
        GLfloat pt_pos[]  = { 28.0f, fy, 6.5f, 1.0f };
        GLfloat pt_diff[] = { 0.75f, 0.68f, 0.35f, 1.0f }; 
        GLfloat pt_spec[] = { 0.30f, 0.25f, 0.10f, 1.0f };
        GLfloat pt_amb[]  = { 0.0f,  0.0f,  0.0f,  1.0f };
        glLightfv(ptLights[f], GL_POSITION, pt_pos);
        glLightfv(ptLights[f], GL_DIFFUSE,  pt_diff);
        glLightfv(ptLights[f], GL_SPECULAR, pt_spec);
        glLightfv(ptLights[f], GL_AMBIENT,  pt_amb);
        glLightf (ptLights[f], GL_CONSTANT_ATTENUATION,  0.7f);
        glLightf (ptLights[f], GL_LINEAR_ATTENUATION,    0.08f);
        glLightf (ptLights[f], GL_QUADRATIC_ATTENUATION, 0.010f);
        glEnable (ptLights[f]);
    }
}

// Setel ulang posisi light yang world-space setiap frame 
void updateLightPositions() {
    GLfloat moon_pos[] = { 0.4f, 1.0f, 0.3f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, moon_pos);
    GLfloat gnd_pos[] = { 0.0f, -1.0f, 0.0f, 0.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, gnd_pos);

    GLenum ptLights[] = { GL_LIGHT2, GL_LIGHT3, GL_LIGHT4 };
    for (int f = 0; f < NUM_FLOORS; f++) {
        float fy = f * FLOOR_HEIGHT + 3.5f;
        GLfloat pt_pos[] = { 28.0f, fy, 6.5f, 1.0f };
        glLightfv(ptLights[f], GL_POSITION, pt_pos);
    }
}

void registerCollider(float x, float z, float w, float d,
                      float yMin, float yMax) {
    BoundingBox b;
    b.xMin = x;      b.xMax = x + w;
    b.zMin = z - d;  b.zMax = z;
    b.yMin = yMin;   b.yMax = yMax;
    colliders.push_back(b);
}

void handleStairs() {
    bool inX1   = (playerX >= 32.0f && playerX <= 34.0f);
    bool inX2   = (playerX >  34.0f && playerX <= 36.0f);
    bool inXall = (playerX >= 32.0f && playerX <= 36.0f);
    bool inZs   = (playerZ <= -1.0f && playerZ >= -4.6f);
    bool inZb   = (playerZ <  -4.6f && playerZ >= -6.6f);

    bool  inStair = false;
    float targetY = playerY;

    for (int f = 0; f < NUM_FLOORS; f++) {
        float base   = f * FLOOR_HEIGHT;
        float yStart = base + 0.8f;
        float yBord  = base + 3.2f;
        float yEnd   = base + FLOOR_HEIGHT + 0.8f;

        if (inX1 && inZs &&
            playerY >= yStart - 0.4f && playerY <= yBord + 0.4f) {
            float t = (-1.0f - playerZ) / 3.6f;
            targetY = yStart + t * (yBord - yStart);
            inStair = true; break;
        }
        if (inXall && inZb &&
            playerY >= yBord - 0.4f && playerY <= yBord + 0.4f) {
            targetY = yBord;
            inStair = true; break;
        }
        if (f < NUM_FLOORS - 1 &&
            inX2 && inZs &&
            playerY >= yBord - 0.4f && playerY <= yEnd + 0.4f) {
            float t = (playerZ - (-4.6f)) / 3.6f;
            targetY = yBord + t * (yEnd - yBord);
            inStair = true; break;
        }
    }

    if (inStair) {
        playerY = targetY;
    } else {
        if (playerZ > -1.0f) {
            float bestY = 0.8f;
            for (int f = NUM_FLOORS - 1; f >= 0; f--) {
                float fy = f * FLOOR_HEIGHT + 0.8f;
                if (playerY >= fy - 0.3f) { bestY = fy; break; }
            }
            playerY = bestY;
        }
    }
}

void buildPhysicalWorld() {
    colliders.clear();
    for (int f = 0; f < NUM_FLOORS; f++) {
        float yBot = f * FLOOR_HEIGHT;
        float yTop = yBot + FLOOR_HEIGHT;

        registerCollider(0.0f,  3.9f, 56.0f, 0.2f, yBot, yTop);
        registerCollider(0.0f, -9.8f, 56.0f, 0.2f, yBot, yTop);

        for (int i = 0; i < 4; i++) {
            float sx = i * 8.0f;
            registerCollider(sx, 0.0f, 0.2f, 10.0f, yBot, yTop);
            if (i % 2 == 0) {
                registerCollider(sx,        0.0f, 1.0f, 0.2f, yBot, yTop);
                registerCollider(sx + 3.0f, 0.0f, 5.0f, 0.2f, yBot, yTop);
            } else {
                registerCollider(sx,        0.0f, 5.0f, 0.2f, yBot, yTop);
                registerCollider(sx + 7.0f, 0.0f, 1.0f, 0.2f, yBot, yTop);
            }
        }

        for (int i = 5; i <= 6; i++) {
            float sx = (i - 1) * 8.0f + 8.0f;
            registerCollider(sx, 0.0f, 0.2f, 10.0f, yBot, yTop);
            if (i == 5) {
                registerCollider(sx,        0.0f, 1.0f, 0.2f, yBot, yTop);
                registerCollider(sx + 3.0f, 0.0f, 5.0f, 0.2f, yBot, yTop);
            } else {
                registerCollider(sx,        0.0f, 5.0f, 0.2f, yBot, yTop);
                registerCollider(sx + 7.0f, 0.0f, 1.0f, 0.2f, yBot, yTop);
            }
        }
    }
}

bool checkCollision(float nextX, float nextZ) {
    for (size_t i = 0; i < colliders.size(); i++) {
        const BoundingBox& b = colliders[i];
        if (playerY + 1.6f < b.yMin || playerY > b.yMax) continue;
        if (nextX + pRadius > b.xMin && nextX - pRadius < b.xMax &&
            nextZ + pRadius > b.zMin && nextZ - pRadius < b.zMax)
            return true;
    }
    return false;
}
