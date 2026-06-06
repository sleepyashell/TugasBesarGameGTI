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
    TEX_FLOOR_TILE = 0,   
    TEX_WALL,             
    TEX_POSTER_1,         
    TEX_POSTER_2,         
    TEX_POSTER_3,         
    TEX_POSTER_4,         
    TEX_GROUND,
    NUM_TEXTURES
};





GLuint loadBMP(const char* filename);

void initTextures();
void bindTexture(TextureID texID);
void unbindTexture();
void cleanupTextures();





void drawPoster(float x, float y, float z, float w, float h, char axis, GLuint texID, float alpha = 1.0f);
void initPosters();
void drawRoomPosters(int f, int roomIndex);

#endif
