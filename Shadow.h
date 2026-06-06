#ifndef SHADOW_H
#define SHADOW_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif



void makeShadowMatrix(float shadowMat[16], float groundPlane[4], float lightPos[4]);


void beginShadow();


void endShadow();


extern float g_shadowMatrix[16];


void updateShadowMatrix(float floorY);

extern bool g_isShadowPass;
#endif
