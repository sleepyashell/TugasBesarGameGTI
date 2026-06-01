#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>
#include "Input.h"
#include "World.h"
#include "Cutscene.h"
#include "Menu.h"

extern float playerX;
extern float playerZ;
extern float playerSpeed;
extern float playerAngle;
extern float targetAngle;
extern bool  isWalking;
extern float walkTimer;

// Variabel kontrol transisi pergerakan sesuai standarisasi modul 5 & 6
int deltaMoveX = 0;
int deltaMoveZ = 0;

void pressNormalKeys(unsigned char key, int x, int y) {
    // Handle cutscene ENTER input
    if (key == 13) {  // ENTER
        if (cutsceneManager.isRunning()) {
            cutsceneManager.advanceDialog();
            return;
        }
    }
    
    switch (key) {
        // Kontrol Sumbu Z (Maju Mundur)
        case 'w': case 'W': deltaMoveZ = -1; break;
        case 's': case 'S': deltaMoveZ =  1; break;
        
        // Kontrol Sumbu X (Kiri Kanan)
        case 'a': case 'A': deltaMoveX = -1; break;
        case 'd': case 'D': deltaMoveX =  1; break;
        
        // Tombol ESC (ASCII 27) untuk keluar game
        case 27: 
            exit(0); 
            break;
    }
}

void releaseNormalKeys(unsigned char key, int x, int y) {
    // Ignore key releases during cutscene
    if (cutsceneManager.isRunning()) {
        return;
    }
    
    switch (key) {
        case 'w': case 'W':
            if (deltaMoveZ < 0) deltaMoveZ = 0; break;
        case 's': case 'S':
            if (deltaMoveZ > 0) deltaMoveZ = 0; break;
            
        case 'a': case 'A':
            if (deltaMoveX < 0) deltaMoveX = 0; break;
        case 'd': case 'D':
            if (deltaMoveX > 0) deltaMoveX = 0; break;
    }
}


void inputMovement() {
    // Disable movement during cutscene
    if (cutsceneManager.isRunning()) {
        isWalking = false;
        return;
    }
    
    if (deltaMoveX != 0 || deltaMoveZ != 0) {
        isWalking = true;
        walkTimer += 0.15f;
        
        // Hitung sudut target rotasi berdasarkan nilai delta
        targetAngle = atan2f((float)deltaMoveX, (float)deltaMoveZ) * 180.0f / 3.14159f;
        
        // Normalisasi kecepatan vektor gerak diagonal
        float mx = (float)deltaMoveX;
        float mz = (float)deltaMoveZ;
        float mag = sqrt(mx * mx + mz * mz);
        
        float sx = (mx / mag) * playerSpeed;
        float sz = (mz / mag) * playerSpeed;
        
        float origX = playerX;
        float origZ = playerZ;

        // Coba gerak X dengan validasi fisik collider
        if (!checkCollision(origX + sx, origZ)) {
            playerX = origX + sx;
        }
        // Coba gerak Z dengan validasi fisik collider
        if (!checkCollision(origX, origZ + sz)) {
            playerZ = origZ + sz;
        }
    } else {
        isWalking = false;
    }

    // Animasi rotasi hadap halus player
    float diff = targetAngle - playerAngle;
    if (diff > 180.0f)  diff -= 360.0f;
    if (diff < -180.0f) diff += 360.0f;
    playerAngle += diff * 0.15f;

    // Sinkronisasi sistem tinggi tangga
    handleStairs();
}

void inputMenu() {
    // Sementara kosong 
}

void handleAllInput() {
    inputMovement();
    inputMenu();
}
