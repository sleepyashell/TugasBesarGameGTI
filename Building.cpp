#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "Building.h"
#include "Material.h"
#include "Drawing.h"
#include "World.h"

// ==========================================
// MODULE BANGUNAN
// ==========================================

void drawCorridorInterior(float width, float depth) {
    matFloor();   drawBlock(width, 0.05f, depth);  // lantai koridor Z=4..0
    glPushMatrix();
        glTranslatef(0, 4.0f, 0);
        matFloor();   drawBlock(width, 0.05f, depth);
        glTranslatef(0, 0.05f, 0);
        matConcrete(); drawBlock(width, 0.95f, depth);  // plafon koridor
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
        drawBlock(PILLAR_W, 4.0f, 0.2f);
    glPopMatrix();

    // Tiang tengah
    for (int i = 1; i < jumlahSegmen; i++) {
        float x = i * SEGMENT - (PILLAR_W / 2.0f);
        glPushMatrix();
            glTranslatef(x, 0.0f, 0.0f);
            if (alpha < 1.0f) glColor4f(0.5f, 0.5f, 0.5f, alpha);
            else matPillar();
            drawBlock(PILLAR_W, 4.0f, 0.2f);
        glPopMatrix();
    }

    // Tiang kanan
    glPushMatrix();
        glTranslatef(width - PILLAR_W, 0.0f, 0.0f);
        if (alpha < 1.0f) glColor4f(0.5f, 0.5f, 0.5f, alpha);
        else matPillar();
        drawBlock(PILLAR_W, 4.0f, 0.2f);
    glPopMatrix();

    if (alpha < 1.0f) {
        glEnable(GL_LIGHTING);
        glDepthMask(GL_TRUE);
    }
}

// Render 1 ruangan - HANYA dinding kanan (dinding kiri = dinding kanan ruangan sebelah)
// Dinding kiri paling ujung gedung ditambahin manual di drawOneLantai
void drawRoomInterior(float width, float depth) {
    matFloor();   drawBlock(width, 0.05f, depth);
    drawDeskSet(width, depth);

    // Tembok belakang
    glPushMatrix();
        glTranslatef(0, 0, -depth);
        matConcrete(); drawBlock(width, 4.0f, 0.2f);
    glPopMatrix();

    // Plafon
    glPushMatrix();
        glTranslatef(0, 4.0f, 0);
        matFloor();    drawBlock(width, 0.05f, depth);
        glTranslatef(0, 0.05f, 0);
        matConcrete(); drawBlock(width, 0.95f, depth);
    glPopMatrix();

    // TIDAK ADA dinding samping - dihandle di drawOneLantai
}

void drawStairArea(float width, float depth, bool isLastFloor) {
    if (isLastFloor) {
        glPushMatrix();
            glTranslatef(4, 0, 0);
            matFloor(); drawBlock(width, 0.05f, depth);
        glPopMatrix();
        // Hanya dinding kanan
        glPushMatrix();
            glTranslatef(width - 0.2f, 0, 0);
            matPillar(); drawBlock(0.2f, 4.0f, depth - 0.2f);
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
        matFloor(); drawBlock(width, 0.05f, depth);
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

        // Koridor
        glPushMatrix();
            glTranslatef(0, 0, 4);
            drawCorridorInterior(56, 4);
        glPopMatrix();

        glPushMatrix();
            // Lantai + plafon + tembok belakang semua room sekaligus
            for (int i = 0; i < 4; i++) {
                drawRoomInterior(8, 10);
                glTranslatef(8, 0, 0);
            }
            drawStairArea(8, 10, isLastFloor);
            if (hasStairDown) drawStairDown();
            glTranslatef(8, 0, 0);
            for (int i = 0; i < 2; i++) {
                drawRoomInterior(8, 10);
                glTranslatef(8, 0, 0);
            }
        glPopMatrix();

        // Dinding samping: 1 blok panjang, tidak ada overlap sama sekali
        // Kiri (X=0), dari Z=0 ke Z=-10
        matPillar();
        // Kiri
		glPushMatrix();
    		glTranslatef(0, 0, -0.01f);
    		matPillar(); drawBlock(0.2f, 4.0f, 9.99f);
		glPopMatrix();

		// Kanan
		glPushMatrix();
    		glTranslatef(55.8f, 0, -0.01f);
    		matPillar(); drawBlock(0.2f, 4.0f, 9.99f);
		glPopMatrix();

        // Sekat antar ruangan: 1 blok per sekat, bukan 2
        // Posisi sekat di X=8, 16, 24, 40, 48 (skip area tangga X=32..40)
        float sekatX[] = { 8.0f, 16.0f, 24.0f, 32.0f, 40.0f, 48.0f };
		for (int i = 0; i < 6; i++) {   // <-- 6 bukan 5
    		glPushMatrix();
        		glTranslatef(sekatX[i] - 0.1f, 0, -0.01f);
        		matPillar(); drawBlock(0.2f, 4.0f, 9.99f);
    		glPopMatrix();
		}

    glPopMatrix();
}

void drawRuangGedung() {
    for (int f = 0; f < NUM_FLOORS; f++) {
        bool isLastFloor  = (f == NUM_FLOORS - 1);
        bool hasStairDown = (f > 0);
        drawOneLantai(f * FLOOR_HEIGHT, isLastFloor, hasStairDown);
    }
}

// Gambar rantai/papan kayu yang menandakan pintu terkunci
void drawLockedDoorMarker(float alpha) {
    glDisable(GL_LIGHTING);
    
    // Warna rantai besi gelap
    glColor4f(0.15f, 0.15f, 0.18f, alpha);
    
    // Rantai horizontal
    glPushMatrix();
        glTranslatef(0.5f, 1.2f, 0.08f);
        drawBlock(1.0f, 0.08f, 0.06f);
    glPopMatrix();
    
    // Rantai vertikal (gembok di tengah)
    glPushMatrix();
        glTranslatef(0.9f, 0.8f, 0.08f);
        drawBlock(0.12f, 0.5f, 0.06f);
    glPopMatrix();
    
    // Gembok
    glPushMatrix();
        glTranslatef(0.85f, 1.0f, 0.1f);
        glColor4f(0.25f, 0.25f, 0.28f, alpha);
        glutSolidCube(0.15f);
    glPopMatrix();
    
    // Papan merah (tanda terkunci)
    glColor4f(0.6f, 0.1f, 0.1f, alpha * 0.8f);
    glPushMatrix();
        glTranslatef(0.5f, 1.8f, 0.08f);
        drawBlock(0.8f, 0.15f, 0.04f);
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
}
