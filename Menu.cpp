#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdio>
#include <cstdlib>
#include "Menu.h"
#include "Texture.h"

GameState gameState = STATE_MENU;
int selectedMenuItem = 0;

// Forward declaration from Main.cpp
extern void startIntroDialog();
GLuint menuPlayHighlightTex = 0;
GLuint menuExitHighlightTex = 0;

void loadMenuTextures() {
    // Hanya load 2 file yang sudah Anda sediakan
    menuPlayHighlightTex = loadBMP("Assets\\menuplayhighlight.bmp");
    menuExitHighlightTex = loadBMP("Assets\\menuexithighlight.bmp");
}

void cleanupMenuTextures() {
    if (menuPlayHighlightTex) glDeleteTextures(1, &menuPlayHighlightTex);
    if (menuExitHighlightTex) glDeleteTextures(1, &menuExitHighlightTex);
}

void initMenu() {
    gameState = STATE_MENU;
    selectedMenuItem = 0;
    loadMenuTextures();
}

void resetMenu() {
    gameState = STATE_MENU;
    selectedMenuItem = 0;
}

void drawQuad(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(x, y);
    glTexCoord2f(1, 1); glVertex2f(x + w, y);
    glTexCoord2f(1, 0); glVertex2f(x + w, y + h);
    glTexCoord2f(0, 0); glVertex2f(x, y + h);
    glEnd();
}

void drawMenu() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Setup 2D projection fullscreen
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1280, 720, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Pilih texture berdasarkan menu yang aktif
    GLuint activeTexture = (selectedMenuItem == 0) ? menuPlayHighlightTex : menuExitHighlightTex;
    
    if (activeTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, activeTexture);
        glColor3f(1.0f, 1.0f, 1.0f);
        // Gambar fullscreen 1280x720
        drawQuad(0, 0, 1280, 720);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    } else {
        // Fallback jika texture gagal load
        glColor3f(0.05f, 0.05f, 0.08f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(1280, 0);
        glVertex2f(1280, 720);
        glVertex2f(0, 720);
        glEnd();
    }
    
    // Restore projection
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    
    glutSwapBuffers();
}

void handleMenuInput(int key) {
    switch (key) {
        case 'w':
        case 'W':
            selectedMenuItem = 0;
            break;
            
        case 's':
        case 'S':
            selectedMenuItem = 1;
            break;
            
        case 13:  // ENTER
            if (selectedMenuItem == 0) {
                startIntroDialog();
                gameState = STATE_PLAYING;
            } else if (selectedMenuItem == 1) {
                cleanupMenuTextures();
                exit(0);
            }
            break;
            
        case 27:  // ESC
            cleanupMenuTextures();
            exit(0);
            break;
    }
}

void handleMouseClick(int button, int state, int x, int y) {
    if (gameState != STATE_MENU) return;
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Area Play (perkiraan, sesuaikan dengan posisi tombol di gambar Anda)
        if (y >= 250 && y <= 380 && x >= 450 && x <= 830) {
            startIntroDialog();
            gameState = STATE_PLAYING;
        }
        // Area Exit
        else if (y >= 380 && y <= 510 && x >= 450 && x <= 830) {
            cleanupMenuTextures();
            exit(0);
        }
    }
}
