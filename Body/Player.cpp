#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include "Player.h"
#include "Material.h"
#include "World.h"

float playerX      = -4.0f;
float playerY      = 0.8f;
float playerZ      = 2.0f;
float playerSpeed  = 0.12f;
float pRadius      = 0.25f;
float playerAngle  = 0.0f;
float targetAngle  = 0.0f;
float walkTimer    = 0.0f;
bool  isWalking    = false;
bool playerWon = false;

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
