#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include "Lighting.h"
#include "World.h" // Tetap butuh ini untuk membaca konstanta NUM_FLOORS & FLOOR_HEIGHT

// Deklarasi variabel pendukung efek kedipan
float flickerTimer     = 0.0f;
float flickerIntensity = 1.0f;

// 1. Fungsi Inisialisasi Awal Lampu (Pindahan dari World.cpp)
void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // --- Cahaya 0: Bulan  ---
    GLfloat moon_pos[]  = { 0.4f, 1.0f, 0.3f, 0.0f };
    GLfloat moon_diff[] = { 0.28f, 0.32f, 0.42f, 1.0f }; 
    GLfloat moon_spec[] = { 0.18f, 0.20f, 0.28f, 1.0f };
    GLfloat moon_amb[]  = { 0.0f,  0.0f,  0.0f,  1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, moon_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  moon_diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, moon_spec);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  moon_amb);
    glEnable(GL_LIGHT0);

    // --- Cahaya 1: Ground ---
    GLfloat gnd_pos[]  = { 0.0f, -1.0f, 0.0f, 0.0f };
    GLfloat gnd_diff[] = { 0.12f, 0.05f, 0.04f, 1.0f };
    GLfloat gnd_spec[] = { 0.0f,  0.0f,  0.0f,  1.0f };
    GLfloat gnd_amb[]  = { 0.0f,  0.0f,  0.0f,  1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, gnd_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  gnd_diff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, gnd_spec);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  gnd_amb);
    glEnable(GL_LIGHT1);

    // Ambient global lingkungan horor
    GLfloat global_amb[] = { 0.10f, 0.10f, 0.13f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_amb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Setup awal poin lampu per lantai koridor
    GLenum ptLights[] = { GL_LIGHT2, GL_LIGHT3, GL_LIGHT4 };
    for (int f = 0; f < NUM_FLOORS; f++) {
        float fy = f * FLOOR_HEIGHT + 3.5f;
        GLfloat pt_pos[]  = { 28.0f, fy, 6.5f, 1.0f };
        GLfloat pt_diff[] = { 0.75f, 0.68f, 0.35f, 1.0f }; 
        GLfloat pt_spec[] = { 0.30f, 0.25f, 0.10f, 1.0f };
        GLfloat pt_amb[]  = { 0.0f,  0.0f,  0.0f,  1.0f };
        glLightfv(ptLights[f], GL_POSITION, pt_pos);
        glLightfv(ptLights[f], GL_DIFFUSE,  pt_diff);
        glLightfv(ptLights[f], GL_SPECULAR, pt_spec);
        glLightfv(ptLights[f], GL_AMBIENT,  pt_amb);
        glLightf (ptLights[f], GL_CONSTANT_ATTENUATION,  0.7f);
        glLightf (ptLights[f], GL_LINEAR_ATTENUATION,    0.08f);
        glLightf (ptLights[f], GL_QUADRATIC_ATTENUATION, 0.010f);
        glEnable (ptLights[f]);
    }
}

// 2. Fungsi Internal Posisi Lampu (Pindahan dari World.cpp)
void updateLightPositions() {
    GLfloat moon_pos[] = { 0.4f, 1.0f, 0.3f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, moon_pos);
    GLfloat gnd_pos[] = { 0.0f, -1.0f, 0.0f, 0.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, gnd_pos);

    GLenum ptLights[] = { GL_LIGHT2, GL_LIGHT3, GL_LIGHT4 };
    for (int f = 0; f < NUM_FLOORS; f++) {
        float fy = f * FLOOR_HEIGHT + 3.5f;
        GLfloat pt_pos[] = { 28.0f, fy, 6.5f, 1.0f };
        glLightfv(ptLights[f], GL_POSITION, pt_pos);
    }
}

// 3. Loop Utama Pengendali Kedipan Lampu per Frame (Gabungan dari Main.cpp)
void handleAllLighting() {
    flickerTimer += 0.05f;
    flickerIntensity = 0.75f
        + 0.15f * sinf(flickerTimer * 7.3f)
        + 0.08f * sinf(flickerTimer * 23.1f)
        + 0.04f * sinf(flickerTimer * 57.9f);
        
    if (flickerIntensity < 0.0f) flickerIntensity = 0.0f;

    // Segarkan koordinat posisi lampu di dunia 3D
    updateLightPositions();

    // Setel ulang kecerahan warna lampu sesuai nilai flicker intensity terbaru
    GLenum ptLights[] = { GL_LIGHT2, GL_LIGHT3, GL_LIGHT4 };
    for (int f = 0; f < NUM_FLOORS; f++) {
        GLfloat pt_diff[] = {
            0.75f * flickerIntensity,
            0.68f * flickerIntensity,
            0.35f * flickerIntensity, 1.0f
        };
        glLightfv(ptLights[f], GL_DIFFUSE, pt_diff);
    }
}
