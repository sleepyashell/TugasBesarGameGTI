#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdio>
#include <cstdlib>
#include "Menu.h"
#include "Texture.h"
#include "Cutscene.h"
#include "Sound.h"

GameState gameState = STATE_MENU;
int selectedMenuItem = 0;
GLuint menuPlayHighlightTex = 0;
GLuint menuExitHighlightTex = 0;
GLuint winHomeHighlightTex  = 0;
GLuint winExitHighlightTex  = 0;

void loadMenuTextures() {
    
    menuPlayHighlightTex = loadBMP("Assets//menuplayhighlight.bmp");
    menuExitHighlightTex = loadBMP("Assets/menuexithighlight.bmp");
    winHomeHighlightTex  = loadBMP("Assets/winscreenhomehighlight.bmp");
    winExitHighlightTex  = loadBMP("Assets/winscreenexithighlight.bmp");
}

void cleanupMenuTextures() {
    if (menuPlayHighlightTex) glDeleteTextures(1, &menuPlayHighlightTex);
    if (menuExitHighlightTex) glDeleteTextures(1, &menuExitHighlightTex);
    if (winHomeHighlightTex)  glDeleteTextures(1, &winHomeHighlightTex);
    if (winExitHighlightTex)  glDeleteTextures(1, &winExitHighlightTex);
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
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1280, 720, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    GLuint activeTexture = (selectedMenuItem == 0) ? menuPlayHighlightTex : menuExitHighlightTex;
    
    if (activeTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, activeTexture);
        glColor3f(1.0f, 1.0f, 1.0f);
        
        drawQuad(0, 0, 1280, 720);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    } else {
        
        glColor3f(0.05f, 0.05f, 0.08f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(1280, 0);
        glVertex2f(1280, 720);
        glVertex2f(0, 720);
        glEnd();
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glutSwapBuffers();
}

void drawWinScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1280, 720, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    GLuint activeTexture = (selectedMenuItem == 0) ? winHomeHighlightTex : winExitHighlightTex;

    if (activeTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, activeTexture);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawQuad(0, 0, 1280, 720);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
  	glutSwapBuffers();
}
