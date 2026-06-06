#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstring>
#include "Shadow.h"

float g_shadowMatrix[16];
bool g_isShadowPass = false;

void makeShadowMatrix(float shadowMat[16], float groundPlane[4], float lightPos[4]) {
    float dot = groundPlane[0] * lightPos[0] +
                groundPlane[1] * lightPos[1] +
                groundPlane[2] * lightPos[2] +
                groundPlane[3] * lightPos[3];

    shadowMat[0]  = dot - lightPos[0] * groundPlane[0];
    shadowMat[4]  =     - lightPos[0] * groundPlane[1];
    shadowMat[8]  =     - lightPos[0] * groundPlane[2];
    shadowMat[12] =     - lightPos[0] * groundPlane[3];

    shadowMat[1]  =     - lightPos[1] * groundPlane[0];
    shadowMat[5]  = dot - lightPos[1] * groundPlane[1];
    shadowMat[9]  =     - lightPos[1] * groundPlane[2];
    shadowMat[13] =     - lightPos[1] * groundPlane[3];

    shadowMat[2]  =     - lightPos[2] * groundPlane[0];
    shadowMat[6]  =     - lightPos[2] * groundPlane[1];
    shadowMat[10] = dot - lightPos[2] * groundPlane[2];
    shadowMat[14] =     - lightPos[2] * groundPlane[3];

    shadowMat[3]  =     - lightPos[3] * groundPlane[0];
    shadowMat[7]  =     - lightPos[3] * groundPlane[1];
    shadowMat[11] =     - lightPos[3] * groundPlane[2];
    shadowMat[15] = dot - lightPos[3] * groundPlane[3];
}

void updateShadowMatrix(float floorY) {
    float groundPlane[4] = { 0.0f, 1.0f, 0.0f, -floorY };
    float lightPos[4] = { 0.4f, 1.0f, 0.3f, 0.0f };
    makeShadowMatrix(g_shadowMatrix, groundPlane, lightPos);
}

void beginShadow() {
	g_isShadowPass = true;
	glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor4f(0.0f, 0.0f, 0.05f, 0.35f);  
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);                
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-2.0f, -2.0f);
    glPushMatrix();
    glMultMatrixf(g_shadowMatrix);
}

void endShadow() {
	g_isShadowPass = false;
	glPopMatrix();
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_COLOR_MATERIAL);
    glDepthMask(GL_TRUE);                 
    glEnable(GL_LIGHTING);
}
