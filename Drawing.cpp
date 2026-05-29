#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Drawing.h"
#include "Material.h"
// ==========================================
// RENDERING HELPERS
// ==========================================

// drawBlock 
void drawBlock(float w, float h, float d) {
    glPushMatrix();
        glTranslatef(w / 2.0f, h / 2.0f, -d / 2.0f);
        glScalef(w, h, d);
        glutSolidCube(1.0);
    glPopMatrix();
}

// Wireframe outline 
void drawBlockOutline(float w, float h, float d, float alpha) {
    glDisable(GL_LIGHTING);
    glColor4f(0, 0, 0, alpha * 0.35f);
    glPushMatrix();
        glTranslatef(w / 2.0f, h / 2.0f, -d / 2.0f);
        glScalef(w, h, d);
        glutWireCube(1.002);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

// Gambar ground plane berkotak
void drawGround() {
    matGround();
    // Panel tanah besar
    glPushMatrix();
        glTranslatef(-10.0f, 0.0f, -30.0f);
        drawBlock(80.0f, 0.05f, 60.0f);
    glPopMatrix();

    // Garis grid tipis 
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0.35f, 0.35f, 0.38f);
    for (int i = -10; i <= 70; i++) {
        glVertex3f((float)i, 0.06f, -30); glVertex3f((float)i, 0.06f, 30);
        glVertex3f(-10, 0.06f, (float)i); glVertex3f(70, 0.06f, (float)i);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// ==========================================
// POHON
// ==========================================

// Gambar pohon (1 pohon)
void drawTree(float x, float z, float trunkH, float crownR) {
    glPushMatrix();
    glTranslatef(x, 0, z);

    // Batang
    matTrunk();
    glPushMatrix();
        glTranslatef(0, 0, 0);
        glScalef(0.22f, trunkH, 0.22f);
        glTranslatef(0, 0.5f, 0);
        glutSolidCube(1.0);
    glPopMatrix();

    // Daun
    matLeaf();
    for (int layer = 0; layer < 3; layer++) {
        float ly = trunkH * 0.6f + layer * (crownR * 0.55f);
        float lr = crownR * (1.0f - layer * 0.22f);
        glPushMatrix();
            glTranslatef(0, ly, 0);
            glutSolidSphere(lr, 10, 8);
        glPopMatrix();
    }

    glPopMatrix();
}

// Deretan pohon
void drawTrees() {
    // Pohon berjejer tiap 8 unit X di sisi depan koridor
    float treeZ_front =  10.0f;  // baris depan
    float treeZ_back  =   8.0f;  // baris belakang 

    for (int i = 0; i < 8; i++) {
        float tx = 2.0f + i * 7.0f;
        // Hindari area tangga (X=32..36) dan pintu utama
        bool nearStair = (tx > 30.0f && tx < 38.0f);
        float h    = 2.2f + 0.4f * (i % 3);
        float cr   = 1.2f + 0.25f * (i % 2);
        if (!nearStair) {
            drawTree(tx, treeZ_front, h, cr);
        }
        // Baris kedua, digeser sedikit
        float tx2 = 4.0f + i * 7.0f;
        bool nearStair2 = (tx2 > 30.5f && tx2 < 37.5f);
        if (!nearStair2) {
            drawTree(tx2, treeZ_back, h * 0.85f, cr * 0.9f);
        }
    }

    // Pohon sisi kiri dan kanan bangunan
    for (int i = 0; i < 4; i++) {
        float tz = -3.0f - i * 2.5f;
        drawTree(-3.0f, tz, 2.5f + i * 0.3f, 1.3f);
        drawTree(58.0f, tz, 2.5f + i * 0.2f, 1.2f);
    }
}

// ==========================================
// FURNITUR
// ==========================================

// Gambar 1 meja: tabletop + 4 kaki
// Posisi (x, y, z) adalah pojok kiri-depan-bawah kaki meja
void drawDesk(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    const float TW = 1.4f;   // lebar tabletop
    const float TH = 0.08f;  // tebal tabletop
    const float TD = 0.7f;   // kedalaman tabletop
    const float LH = 0.72f;  // tinggi kaki
    const float LW = 0.06f;  // lebar kaki

    // Tabletop
    matDesk();
    glPushMatrix();
        glTranslatef(0.0f, LH, 0.0f);
        drawBlock(TW, TH, TD);
    glPopMatrix();

    // 4 kaki meja
    matDeskLeg();
    float ox = 0.08f;  // offset kaki dari tepi tabletop
    float oz = 0.08f;

    // Kaki kiri-depan
    glPushMatrix();
        glTranslatef(ox, 0.0f, -oz);
        drawBlock(LW, LH, LW);
    glPopMatrix();
    // Kaki kanan-depan
    glPushMatrix();
        glTranslatef(TW - ox - LW, 0.0f, -oz);
        drawBlock(LW, LH, LW);
    glPopMatrix();
    // Kaki kiri-belakang
    glPushMatrix();
        glTranslatef(ox, 0.0f, -(TD - oz - LW));
        drawBlock(LW, LH, LW);
    glPopMatrix();
    // Kaki kanan-belakang
    glPushMatrix();
        glTranslatef(TW - ox - LW, 0.0f, -(TD - oz - LW));
        drawBlock(LW, LH, LW);
    glPopMatrix();

    glPopMatrix();
}

// Atur deretan meja dalam 1 ruang kelas
// Layout: 3 kolom x 3 baris = 9 meja per ruangan
void drawDeskSet(float roomW, float roomD) {
    const int COLS = 3;
    const int ROWS = 3;

    // Spasi antar meja
    float spacingX = (roomW - 0.4f) / (float)COLS;  // bagi lebar ruang
    float spacingZ = (roomD - 2.5f) / (float)ROWS;  // bagi kedalaman (sisakan jalan di depan)

    float startX = 0.5f;         // jarak dari dinding kiri
    float startZ = -1.8f;        // mulai dari dekat dinding belakang

    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS; row++) {
            float mx = startX + col * spacingX;
            float mz = startZ - row * spacingZ;
            drawDesk(mx, 0.05f, mz);  // y=0.05 agar duduk di atas lantai ruangan
        }
    }
}


