#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef DRAWING_H
#define DRAWING_H

void drawBlock(float w, float h, float d);
void drawWallBlock(float w, float h, float d);  // drawBlock dengan UV untuk texture dinding
void drawBlockOutline(float w, float h, float d, float alpha = 1.0f);
void drawGround();
void drawTree(float x, float z, float trunkH = 2.5f, float crownR = 1.4f);
void drawTrees();
void drawDesk(float x, float y, float z);
void drawDeskSet(float roomW, float roomD);

#endif
