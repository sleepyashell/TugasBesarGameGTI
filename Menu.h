#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_WIN
};

extern GameState gameState;
extern int selectedMenuItem;

extern GLuint menuPlayHighlightTex;
extern GLuint menuExitHighlightTex;
extern GLuint winHomeHighlightTex;
extern GLuint winExitHighlightTex;

void initMenu();
void loadMenuTextures();
void drawMenu();
void drawWinScreen();
void handleMouseClick(int button, int state, int x, int y);
void resetMenu();
void cleanupMenuTextures();
void startIntroDialog();

#endif
