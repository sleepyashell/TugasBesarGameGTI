#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef MATERIAL_H
#define MATERIAL_H


void setMaterial(float r, float g, float b, float a,
                 float specR = 0.2f, float specG = 0.2f, float specB = 0.2f,
                 float shininess = 16.0f);
void matConcrete();
void matFloor();
void matCeiling();
void matPillar();
void matStair();
void matDoor();
void matHandle();
void matTrunk();
void matLeaf();
void matPlayer();
void matGround();
void matDesk();
void matDeskLeg();
#endif
