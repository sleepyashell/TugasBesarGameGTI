#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include "Player.h"
#include "Material.h"
#include "World.h"

extern float playerAngle;
extern float walkTimer;
extern bool  isWalking;

void drawPlayer() {
    float swing = isWalking ? sinf(walkTimer) * 30.0f : 0.0f;

    glPushMatrix();
    glTranslatef(playerX, playerY - 0.4f, playerZ);
    glRotatef(playerAngle, 0.0f, 1.0f, 0.0f);

        // Kaki kiri
        setMaterial(0.15f, 0.10f, 0.10f, 1.0f, 0.10f, 0.05f, 0.05f, 16.0f);
        glPushMatrix();
            glTranslatef(-0.12f, 0.25f, 0.0f);
            glRotatef(swing, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -0.25f, 0.0f);
            glScalef(0.18f, 0.5f, 0.18f);
            glutSolidCube(1);
        glPopMatrix();

        // Kaki kanan
        glPushMatrix();
            glTranslatef(0.12f, 0.25f, 0.0f);
            glRotatef(-swing, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -0.25f, 0.0f);
            glScalef(0.18f, 0.5f, 0.18f);
            glutSolidCube(1);
        glPopMatrix();

        // Badan
        setMaterial(0.55f, 0.05f, 0.05f, 1.0f, 0.40f, 0.10f, 0.10f, 48.0f);
        glPushMatrix();
            glTranslatef(0.0f, 0.55f, 0.0f);
            glScalef(0.45f, 0.6f, 0.25f);
            glutSolidCube(1);
        glPopMatrix();

        // Lengan kiri
        glPushMatrix();
            glTranslatef(-0.30f, 0.80f, 0.0f);
            glRotatef(-swing, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -0.25f, 0.0f);
            glScalef(0.15f, 0.55f, 0.15f);
            glutSolidCube(1);
        glPopMatrix();

        // Lengan kanan
        glPushMatrix();
            glTranslatef(0.30f, 0.80f, 0.0f);
            glRotatef(swing, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -0.25f, 0.0f);
            glScalef(0.15f, 0.55f, 0.15f);
            glutSolidCube(1);
        glPopMatrix();

        // Kepala
        setMaterial(0.80f, 0.60f, 0.45f, 1.0f, 0.30f, 0.20f, 0.15f, 32.0f);
        glPushMatrix();
            glTranslatef(0.0f, 1.1f, 0.0f);
            glutSolidSphere(0.2f, 12, 10);
        glPopMatrix();

    glPopMatrix();
}
