#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "Building.h"
#include "Material.h"  // <--- TAMBAHKAN INI (Untuk matFloor, matConcrete, matPillar, dll)
#include "Drawing.h"   // <--- TAMBAHKAN INI (Untuk fungsi drawBlock)
#include "World.h"     // <--- TAMBAHKAN INI (Untuk playerX, playerY, NUM_FLOORS, dll)

// ==========================================
// MODULE BANGUNAN
// ==========================================

void drawCorridorInterior(float width, float depth) {
    matFloor();   drawBlock(width, 0.05f, depth);
    glPushMatrix();
        glTranslatef(0, 4.0f, 0);
        matFloor();   drawBlock(width, 0.05f, depth);
        glTranslatef(0, 0.05f, 0);
        matConcrete(); drawBlock(width, 0.95f, depth);
    glPopMatrix();
}

void drawCorridorFront(float width, float floorY) {
    bool  on    = (playerY >= floorY && playerY < floorY + 4.0f);
    float alpha = on ? 0.35f : 1.0f;

    if (alpha < 1.0f) {
        glDepthMask(GL_FALSE);
        glDisable(GL_LIGHTING);
    }

    // Balok depan bawah
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -0.1f);
        if (alpha < 1.0f) glColor4f(0.6f, 0.6f, 0.6f, alpha);
        else { matConcrete(); }
        drawBlock(width, 1.0f, 0.2f);
    glPopMatrix();

    const float SEGMENT  = 8.0f;
    const float PILLAR_W = 0.4f;
    int jumlahSegmen = (int)(width / SEGMENT);

    // Tiang kiri
    glPushMatrix();
        if (alpha < 1.0f) glColor4f(0.5f, 0.5f, 0.5f, alpha);
        else matPillar();
        drawBlock(PILLAR_W, 4.0f, 0.4f);
    glPopMatrix();

    // Tiang tengah
    for (int i = 1; i < jumlahSegmen; i++) {
        float x = i * SEGMENT - (PILLAR_W / 2.0f);
        glPushMatrix();
            glTranslatef(x, 0.0f, 0.0f);
            if (alpha < 1.0f) glColor4f(0.5f, 0.5f, 0.5f, alpha);
            else matPillar();
            drawBlock(PILLAR_W, 4.0f, 0.4f);
        glPopMatrix();
    }

    // Tiang kanan
    glPushMatrix();
        glTranslatef(width - PILLAR_W, 0.0f, 0.0f);
        if (alpha < 1.0f) glColor4f(0.5f, 0.5f, 0.5f, alpha);
        else matPillar();
        drawBlock(PILLAR_W, 4.0f, 0.4f);
    glPopMatrix();

    if (alpha < 1.0f) {
        glEnable(GL_LIGHTING);
        glDepthMask(GL_TRUE);
    }
}

void drawRoomInterior(float width, float depth) {
    matFloor();   drawBlock(width, 0.05f, depth);
    drawDeskSet(width, depth);
    glPushMatrix();
        glTranslatef(0, 0, -depth);
        matConcrete(); drawBlock(width, 4.0f, 0.2f);
    glPopMatrix();
    matPillar();
    drawBlock(0.2f, 4.0f, depth);
    glPushMatrix();
        glTranslatef(width - 0.2f, 0, 0);
        drawBlock(0.2f, 4.0f, depth);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 4.0f, 0);
        matFloor();    drawBlock(width, 0.05f, depth);
        glTranslatef(0, 0.05f, 0);
        matConcrete(); drawBlock(width, 0.95f, depth);
    glPopMatrix();
}

void drawStairArea(float width, float depth, bool isLastFloor) {
    if (isLastFloor) {
    	glPushMatrix();
            glTranslatef(4, 0, 0);
        	matFloor();    drawBlock(width, 0.05f, depth);
        glPopMatrix();
        matPillar();   drawBlock(0.2f, 4.0f, depth);
        glPushMatrix();
            glTranslatef(width - 0.2f, 0, 0);
            drawBlock(0.2f, 4.0f, depth);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, 0, -depth);
            matConcrete(); drawBlock(width, 4.0f, 0.2f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, 4.0f, 0);
            matFloor();    drawBlock(width, 0.05f, depth);
            glTranslatef(0, 0.05f, 0);
            matConcrete(); drawBlock(width, 0.95f, depth);
        glPopMatrix();
        
        matFloor(); drawBlock(4.0f, 0.05f, 1.0f);
        
        glPushMatrix();
	        glTranslatef(0, -10.0f, 0);
	        matFloor(); drawBlock(width, 0.05f, depth);
        glPopMatrix();
        return;
    }

	glPushMatrix();
            glTranslatef(4, 0, 0);
        	matFloor();    drawBlock(width, 0.05f, depth);
        glPopMatrix();
    

    glPushMatrix();
        glTranslatef(0, 0, -depth);
        matConcrete(); drawBlock(width, FLOOR_HEIGHT, 0.2f);
    glPopMatrix();

    // Flight naik
    for (int i = 0; i < 12; i++) {
        glPushMatrix();
            glTranslatef(0.0f, (float)i * 0.2f, -1.0f - (float)i * 0.3f);
            matStair(); drawBlock(2.0f, 0.2f, 0.3f);
        glPopMatrix();
    }

    // Bordes
    glPushMatrix();
        glTranslatef(0.0f, 2.4f, -4.6f);
        matStair(); drawBlock(4.0f, 0.2f, 2.0f);
    glPopMatrix();

    // Sisi kanan (X=0..4)
    matFloor(); drawBlock(4.0f, 0.05f, 1.0f);

    glPushMatrix();
        glTranslatef(0.0f, FLOOR_HEIGHT, -6.0f);
        matFloor(); drawBlock(4.0f, 0.05f, depth - 6.0f);
    glPopMatrix();
}

void drawStairDown() {
    for (int i = 0; i < 12; i++) {
        glPushMatrix();
            glTranslatef(2.0f, -(float)(i + 1) * 0.2f, -1.0f - (float)i * 0.3f);
            matStair(); drawBlock(2.0f, 0.2f, 0.3f);
        glPopMatrix();
    }
}

void drawDoubleDoor(float alpha) {
    if (alpha < 1.0f) {
        glDisable(GL_LIGHTING);
        glPushMatrix();
            glTranslatef(0.02f, 0, -0.05f);
            glColor4f(0.45f, 0.28f, 0.15f, alpha);
            drawBlock(1.0f, 2.5f, 0.1f);
            glTranslatef(0.85f, 1.0f, 0.02f);
            glColor4f(0.8f, 0.8f, 0.78f, alpha);
            drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(1.03f, 0, -0.05f);
            glColor4f(0.45f, 0.28f, 0.15f, alpha);
            drawBlock(1.0f, 2.5f, 0.1f);
            glTranslatef(0.1f, 1.0f, 0.02f);
            glColor4f(0.8f, 0.8f, 0.78f, alpha);
            drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    } else {
        glPushMatrix();
            glTranslatef(0.02f, 0, -0.05f);
            matDoor();   drawBlock(1.0f, 2.5f, 0.1f);
            glTranslatef(0.85f, 1.0f, 0.02f);
            matHandle(); drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(1.03f, 0, -0.05f);
            matDoor();   drawBlock(1.0f, 2.5f, 0.1f);
            glTranslatef(0.1f, 1.0f, 0.02f);
            matHandle(); drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
    }
}

void drawFrontWall(float width, float depth, float startX, bool isFlipped,
                   float floorY) {
    bool  insideZ   = (playerZ < 0.5f);
    bool  insideX   = (playerX >= startX && playerX <= startX + width);
    bool  sameFloor = (playerY >= floorY && playerY < floorY + FLOOR_HEIGHT);
    float alpha     = (insideZ && insideX && sameFloor) ? 0.35f : 1.0f;

    if (alpha < 1.0f) {
        glDepthMask(GL_FALSE);
        glDisable(GL_LIGHTING);
    }

    float doorX = isFlipped ? 5.0f : 1.0f;

    if (alpha < 1.0f) {
        glColor4f(0.7f, 0.7f, 0.65f, alpha);
        drawBlock(doorX, 4.0f, 0.2f);
        glPushMatrix();
            glTranslatef(doorX + 2.0f, 0, 0);
            drawBlock(width - (doorX + 2.0f), 4.0f, 0.2f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(doorX, 2.5f, 0);
            drawBlock(2.0f, 1.5f, 0.2f);
        glPopMatrix();
    } else {
        matConcrete(); drawBlock(doorX, 4.0f, 0.2f);
        glPushMatrix();
            glTranslatef(doorX + 2.0f, 0, 0);
            matConcrete(); drawBlock(width - (doorX + 2.0f), 4.0f, 0.2f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(doorX, 2.5f, 0);
            matConcrete(); drawBlock(2.0f, 1.5f, 0.2f);
        glPopMatrix();
    }

    if (alpha < 1.0f) {
        glEnable(GL_LIGHTING);
        glDepthMask(GL_TRUE);
    }

    glPushMatrix();
        glTranslatef(doorX, 0, 0);
        drawDoubleDoor(alpha);
    glPopMatrix();
}

void drawOneLantai(float offsetY, bool isLastFloor, bool hasStairDown) {
    glPushMatrix();
    glTranslatef(0, offsetY, 0);

        glPushMatrix();
            glTranslatef(0, 0, 4);
            drawCorridorInterior(56, 4);
        glPopMatrix();

        glPushMatrix();
            for (int i = 0; i < 4; i++) {
                drawRoomInterior(8, 10);
                glTranslatef(8, 0, 0);
            }
            drawStairArea(8, 10, isLastFloor);
            if (hasStairDown) {
                drawStairDown();
            }
            glTranslatef(8, 0, 0);
            for (int i = 0; i < 2; i++) {
                drawRoomInterior(8, 10);
                glTranslatef(8, 0, 0);
            }
        glPopMatrix();

    glPopMatrix();
}

void drawRuangGedung() {
    for (int f = 0; f < NUM_FLOORS; f++) {
        bool isLastFloor  = (f == NUM_FLOORS - 1);
        bool hasStairDown = (f > 0);
        drawOneLantai(f * FLOOR_HEIGHT, isLastFloor, hasStairDown);
    }
}
