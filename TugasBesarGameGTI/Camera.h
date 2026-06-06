#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef CAMERA_H
#define CAMERA_H

// Fungsi untuk mengatur posisi sudut pandang kamera (Centering ke Player)
void setupCamera();

#endif
