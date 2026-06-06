#include "Drawing.h"

#include "Material.h"
#include "World.h"
#include "Texture.h"

// drawBlock biasa (tanpa UV, untuk geometry non-texture)
void drawBlock(float w, float h, float d) {
    glPushMatrix();
        glTranslatef(w / 2.0f, h / 2.0f, -d / 2.0f);
        glScalef(w, h, d);
        glutSolidCube(1.0);
    glPopMatrix();
}

// drawWallBlock - sama tapi pake glBegin/GL_QUADS dengan UV
// Dipanggil setelah bindTexture(TEX_WALL)
void drawWallBlock(float w, float h, float d) {
    float su = w / 2.0f;  // 1 tile = 2 unit
    float sv = h / 2.0f;
    float sd = d / 2.0f;

    glBegin(GL_QUADS);
        // Depan (Z=0)
        glNormal3f(0, 0, 1);
        glTexCoord2f(0,  0);  glVertex3f(0, 0, 0);
        glTexCoord2f(su, 0);  glVertex3f(w, 0, 0);
        glTexCoord2f(su, sv); glVertex3f(w, h, 0);
        glTexCoord2f(0,  sv); glVertex3f(0, h, 0);

        // Belakang (Z=-d)
        glNormal3f(0, 0, -1);
        glTexCoord2f(0,  0);  glVertex3f(w, 0, -d);
        glTexCoord2f(su, 0);  glVertex3f(0, 0, -d);
        glTexCoord2f(su, sv); glVertex3f(0, h, -d);
        glTexCoord2f(0,  sv); glVertex3f(w, h, -d);

        // Kiri (X=0)
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0,  0);  glVertex3f(0, 0, -d);
        glTexCoord2f(sd, 0);  glVertex3f(0, 0,  0);
        glTexCoord2f(sd, sv); glVertex3f(0, h,  0);
        glTexCoord2f(0,  sv); glVertex3f(0, h, -d);

        // Kanan (X=w)
        glNormal3f(1, 0, 0);
        glTexCoord2f(0,  0);  glVertex3f(w, 0,  0);
        glTexCoord2f(sd, 0);  glVertex3f(w, 0, -d);
        glTexCoord2f(sd, sv); glVertex3f(w, h, -d);
        glTexCoord2f(0,  sv); glVertex3f(w, h,  0);

        // Atas (Y=h)
        glNormal3f(0, 1, 0);
        glTexCoord2f(0,  0);  glVertex3f(0, h,  0);
        glTexCoord2f(su, 0);  glVertex3f(w, h,  0);
        glTexCoord2f(su, sd); glVertex3f(w, h, -d);
        glTexCoord2f(0,  sd); glVertex3f(0, h, -d);

        // Bawah (Y=0)
        glNormal3f(0, -1, 0);
        glTexCoord2f(0,  0);  glVertex3f(0, 0, -d);
        glTexCoord2f(su, 0);  glVertex3f(w, 0, -d);
        glTexCoord2f(su, sd); glVertex3f(w, 0,  0);
        glTexCoord2f(0,  sd); glVertex3f(0, 0,  0);
    glEnd();
}

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

void drawGround() {
    matGround();
    glPushMatrix();
        glTranslatef(-10.0f, 0.0f, -30.0f);
        drawBlock(80.0f, 0.05f, 60.0f);
    glPopMatrix();

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

void drawTree(float x, float z, float trunkH, float crownR) {
    glPushMatrix();
    glTranslatef(x, 0, z);

    matTrunk();
    glPushMatrix();
        glTranslatef(0, 0, 0);
        glScalef(0.22f, trunkH, 0.22f);
        glTranslatef(0, 0.5f, 0);
        glutSolidCube(1.0);
    glPopMatrix();

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

void drawTrees() {
    float treeZ_front = 10.0f;
    float treeZ_back  =  8.0f;

    for (int i = 0; i < 8; i++) {
        float tx = 2.0f + i * 7.0f;
        bool nearStair = (tx > 30.0f && tx < 38.0f);
        float h  = 2.2f + 0.4f * (i % 3);
        float cr = 1.2f + 0.25f * (i % 2);
        if (!nearStair) drawTree(tx, treeZ_front, h, cr);

        float tx2 = 4.0f + i * 7.0f;
        bool nearStair2 = (tx2 > 30.5f && tx2 < 37.5f);
        if (!nearStair2) drawTree(tx2, treeZ_back, h * 0.85f, cr * 0.9f);
    }

    for (int i = 0; i < 4; i++) {
        float tz = -3.0f - i * 2.5f;
        drawTree(-3.0f, tz, 2.5f + i * 0.3f, 1.3f);
        drawTree(58.0f, tz, 2.5f + i * 0.2f, 1.2f);
    }
}

void drawDesk(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    const float TW = 1.4f;
    const float TH = 0.08f;
    const float TD = 0.7f;
    const float LH = 0.72f;
    const float LW = 0.06f;

    matDesk();
    glPushMatrix();
        glTranslatef(0.0f, LH, 0.0f);
        drawBlock(TW, TH, TD);
    glPopMatrix();

    matDeskLeg();
    float ox = 0.08f;
    float oz = 0.08f;

    glPushMatrix();
        glTranslatef(ox, 0.0f, -oz);
        drawBlock(LW, LH, LW);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(TW - ox - LW, 0.0f, -oz);
        drawBlock(LW, LH, LW);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(ox, 0.0f, -(TD - oz - LW));
        drawBlock(LW, LH, LW);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(TW - ox - LW, 0.0f, -(TD - oz - LW));
        drawBlock(LW, LH, LW);
    glPopMatrix();

    glPopMatrix();
}

void drawDeskSet(float roomW, float roomD) {
    const int COLS = 3;
    const int ROWS = 3;

    float spacingX = (roomW - 0.4f) / (float)COLS;
    float spacingZ = (roomD - 2.5f) / (float)ROWS;
    float startX = 0.5f;
    float startZ = -1.8f;

    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS; row++) {
            float mx = startX + col * spacingX;
            float mz = startZ - row * spacingZ;
            drawDesk(mx, 0.05f, mz);
        }
    }
}
