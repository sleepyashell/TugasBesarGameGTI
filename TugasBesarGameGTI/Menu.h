#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Game state
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED
};

extern GameState gameState;
extern int selectedMenuItem;

// Hanya 2 texture yang diperlukan (single image approach)
extern GLuint menuPlayHighlightTex;
extern GLuint menuExitHighlightTex;

void initMenu();
void loadMenuTextures();
void drawMenu();
void handleMouseClick(int button, int state, int x, int y);
void resetMenu();
void cleanupMenuTextures();
void startIntroDialog();

#endif
