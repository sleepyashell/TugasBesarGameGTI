#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include "World.h"

enum TextureID {
    TEX_FLOOR_TILE = 0,   // floor_title.bmp - ubin lantai
    TEX_WALL,             // wall.bmp - texture dinding
    TEX_POSTER_1,         // poster1.bmp
    TEX_POSTER_2,         // poster2.bmp
    TEX_POSTER_3,         // poster3.bmp
    TEX_POSTER_4,         // poster4.bmp
    NUM_TEXTURES
};

// ==========================================
// TEXTURE MANAGER
// ==========================================

GLuint loadBMP(const char* filename);

void initTextures();
void bindTexture(TextureID texID);
void unbindTexture();
void cleanupTextures();

// ==========================================
// POSTER
// ==========================================

void drawPoster(float x, float y, float z, float w, float h, char axis, GLuint texID, float alpha = 1.0f);
void initPosters();
void drawRoomPosters(int f, int roomIndex);

#endif
