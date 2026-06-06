#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef INPUT_H
#define INPUT_H

void pressNormalKeys(unsigned char key, int x, int y);
void releaseNormalKeys(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);

void inputMovement();

#endif 
