#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdio>
#include <cstdlib>
#include "Menu.h"

GameState gameState = STATE_MENU;
int selectedMenuItem = 0;

// Texture IDs
GLuint menuBackgroundTex = 0;
GLuint menuTitleTex = 0;
GLuint playButtonTex = 0;
GLuint playButtonHighlightTex = 0;
GLuint exitButtonTex = 0;
GLuint exitButtonHighlightTex = 0;

// Helper: load BMP texture
GLuint loadBMPTexture(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }

    // Read BMP header
    unsigned char header[54];
    fread(header, 1, 54, file);

    // Extract width and height
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int bitDepth = *(short*)&header[28];

    // Read pixel data
    int imageSize = width * height * (bitDepth / 8);
    unsigned char* imageData = new unsigned char[imageSize];
    fread(imageData, 1, imageSize, file);
    fclose(file);

    // Create OpenGL texture
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (bitDepth == 24) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, imageData);
    } else if (bitDepth == 32) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, imageData);
    }

    delete[] imageData;
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

void loadMenuTextures() {
    menuBackgroundTex = loadBMPTexture("Assets\\menubackground.bmp");
    menuTitleTex = loadBMPTexture("Assets\\menutitle.bmp");
    playButtonTex = loadBMPTexture("Assets\\playbuttonnothighlight.bmp");
    playButtonHighlightTex = loadBMPTexture("Assets\\playbuttonhighlight.bmp");
    exitButtonTex = loadBMPTexture("Assets\\exitbuttonnothighlight.bmp");
    exitButtonHighlightTex = loadBMPTexture("Assets\\exitbuttonhighlight.bmp");
}

void cleanupMenuTextures() {
    if (menuBackgroundTex) glDeleteTextures(1, &menuBackgroundTex);
    if (menuTitleTex) glDeleteTextures(1, &menuTitleTex);
    if (playButtonTex) glDeleteTextures(1, &playButtonTex);
    if (playButtonHighlightTex) glDeleteTextures(1, &playButtonHighlightTex);
    if (exitButtonTex) glDeleteTextures(1, &exitButtonTex);
    if (exitButtonHighlightTex) glDeleteTextures(1, &exitButtonHighlightTex);
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
    
    // Setup 2D projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1280, 600, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Draw background
    if (menuBackgroundTex) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, menuBackgroundTex);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawQuad(0, 0, 1280, 600);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    } else {
        // Fallback: solid background
        glColor3f(0.1f, 0.1f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(1280, 0);
        glVertex2f(1280, 600);
        glVertex2f(0, 600);
        glEnd();
    }
    
    // Draw title
    if (menuTitleTex) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, menuTitleTex);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawQuad(400, 80, 480, 120);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    
    // Draw PLAY button
    float playY = 250;
    float playX = 350;
    float btnW = 300;
    float btnH = 100;
    
    GLuint playTexture = selectedMenuItem == 0 ? playButtonHighlightTex : playButtonTex;
    if (playTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, playTexture);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawQuad(playX, playY, btnW, btnH);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    
    // Draw EXIT button
    float exitY = 380;
    float exitX = 350;
    
    GLuint exitTexture = selectedMenuItem == 1 ? exitButtonHighlightTex : exitButtonTex;
    if (exitTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, exitTexture);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawQuad(exitX, exitY, btnW, btnH);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
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
        // Check PLAY button (y: 250-350, x: 350-650)
        if (x >= 350 && x <= 650 && y >= 250 && y <= 350) {
            gameState = STATE_PLAYING;
        }
        // Check EXIT button (y: 380-480, x: 350-650)
        else if (x >= 350 && x <= 650 && y >= 380 && y <= 480) {
            cleanupMenuTextures();
            exit(0);
        }
    }
}
