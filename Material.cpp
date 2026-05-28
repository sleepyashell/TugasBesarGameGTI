#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <vector>
#include <cstdlib>

#include "Material.h"

// Set material property 
void setMaterial(float r, float g, float b, float a,
                 float specR, float specG, float specB,
                 float shininess) {
    GLfloat diff[]  = { r,     g,     b,     a };
    GLfloat amb[]   = { r*0.3f, g*0.3f, b*0.3f, a };
    GLfloat spec[]  = { specR, specG, specB, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  spec);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glColor4f(r, g, b, a); 
}

// --- HORROR PALETTE ---
// Concrete / dinding
void matConcrete() { 
	setMaterial(0.22f, 0.23f, 0.20f, 1.0f, 0.08f,0.10f,0.08f,  4.0f); 
}
// Lantai
void matFloor() { 
	setMaterial(0.18f, 0.17f, 0.16f, 1.0f, 0.12f,0.12f,0.10f,  8.0f); 
}
// Plafon
void matCeiling() { 
	setMaterial(0.12f, 0.12f, 0.11f, 1.0f, 0.05f,0.05f,0.05f,  2.0f); 
}
// Tiang / pilar
void matPillar() { 
	setMaterial(0.15f, 0.15f, 0.14f, 1.0f, 0.10f,0.10f,0.10f,  6.0f); 
}
// Tangga
void matStair() { 
	setMaterial(0.20f, 0.18f, 0.16f, 1.0f, 0.15f,0.12f,0.10f, 12.0f); 
}
// Pintu kayu
void matDoor() { 
	setMaterial(0.18f, 0.10f, 0.06f, 1.0f, 0.15f,0.08f,0.05f, 16.0f); 
}
// Handle pintu
void matHandle() { 
	setMaterial(0.35f, 0.22f, 0.10f, 1.0f, 0.30f,0.18f,0.08f, 32.0f); 
}
// Batang pohon
void matTrunk() { 
	setMaterial(0.14f, 0.10f, 0.07f, 1.0f, 0.05f,0.05f,0.05f,  2.0f); 
}
// Daun pohon
void matLeaf() { 
	setMaterial(0.10f, 0.16f, 0.08f, 1.0f, 0.05f,0.08f,0.05f,  2.0f); 
}
// Pemain
void matPlayer() { 
	setMaterial(0.55f, 0.05f, 0.05f, 1.0f, 0.40f,0.10f,0.10f, 48.0f); 
}
// Tanah
void matGround() {
	setMaterial(0.10f, 0.10f, 0.09f, 1.0f, 0.03f,0.03f,0.03f,  2.0f); 
}

// Meja kayu gelap
void matDesk() { 
    setMaterial(0.22f, 0.13f, 0.07f, 1.0f, 0.15f, 0.09f, 0.05f, 20.0f); 
}
// Kaki meja metal berkarat
void matDeskLeg() { 
    setMaterial(0.20f, 0.18f, 0.17f, 1.0f, 0.12f, 0.12f, 0.12f, 10.0f); 
}
