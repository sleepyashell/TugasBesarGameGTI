#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib> 
#include <cmath>    
#include <cstdio>
#include "Bot.h"
#include "World.h"
#include "Sound.h"
#include "Cutscene.h"

EnemyBot ghostBot;
bool isChaseSoundPlaying = false;

const int NUM_NODES = 11;
Waypoint nodes[NUM_NODES] = {
    { 2.0f,  0.0f, 2.0f },   // Node 0: Lantai 1 - Ujung Kiri
    { 33.0f, 0.0f, 2.0f },  // Node 1: Lantai 1 - Depan Tangga (Tengah)
    { 50.0f, 0.0f, 2.0f },  // Node 2: Lantai 1 - Ujung Kanan (FIXED: 50 bukan 54)

    { 2.0f,  4.0f, 2.0f },   // Node 3: Lantai 2 - Ujung Kiri
    { 33.0f, 4.0f, 2.0f },  // Node 4: Lantai 2 - Depan Tangga (Tengah)
    { 50.0f, 4.0f, 2.0f },  // Node 5: Lantai 2 - Ujung Kanan (FIXED: 50 bukan 54)

    { 2.0f,  8.0f, 2.0f },   // Node 6: Lantai 3 - Ujung Kiri
    { 33.0f, 8.0f, 2.0f },  // Node 7: Lantai 3 - Depan Tangga (Tengah)
    { 50.0f, 8.0f, 2.0f },  // Node 8: Lantai 3 - Ujung Kanan (FIXED: 50 bukan 54)

    { 34.0f, 3.2f, -6.0f }, // Node 9: Bordes Tangga Bawah (Lantai 1->2)
    { 34.0f, 8.2f, -6.0f }  // Node 10: Bordes Tangga Atas (Lantai 2->3)
};

void initBot() {
    ghostBot.x = nodes[0].x;    
    ghostBot.y = 0.0f;          
    ghostBot.z = 2.0f;          
    ghostBot.speed = 0.05f;     
    ghostBot.dirX = 1.0f; 
    ghostBot.currentFloor = 0;
    ghostBot.movingRight = true;
    ghostBot.isChasing = false;
    ghostBot.isSearching = false;
    ghostBot.targetNodeIndex = 1; 
}

// =========================================================================
// INTERPOLASI TANGGA: Mengatur Y otomatis berdasarkan kedalaman Z di tangga
// =========================================================================
void handleStairInterpolation() {
    if (ghostBot.x >= 32.0f && ghostBot.x <= 36.0f && ghostBot.z < 2.0f) {
        float base = ghostBot.currentFloor * FLOOR_HEIGHT;
        if (ghostBot.z <= -1.0f && ghostBot.z >= -4.6f) {
            float t = (-1.0f - ghostBot.z) / 3.6f;
            if (t < 0.0f) t = 0.0f; if (t > 1.0f) t = 1.0f;
            ghostBot.y = base + t * 2.4f; 
        } else if (ghostBot.z < -4.6f && ghostBot.z >= -6.6f) {
            ghostBot.y = base + 3.2f;      
        }
    }
}

// =========================================================================
// SCANNING VISION: Logika deteksi player (Depan-Belakang / Sela Tangga)
// =========================================================================
bool scanForPlayer() {
    int playerFloor = (int)(playerY / FLOOR_HEIGHT);
    float distanceX = std::abs(playerX - ghostBot.x);
    bool playerInStairArea = (playerZ <= 0.5f && playerX >= 31.0f && playerX <= 37.0f);

    // Deteksi di koridor utama (Satu Lantai)
    if (ghostBot.currentFloor == playerFloor && ghostBot.z >= 2.0f && playerZ > 0.5f) {
        float toPlayerX = playerX - ghostBot.x;
        float dotProduct = ghostBot.dirX * toPlayerX;

        if (dotProduct > 0.0f) {
            if (distanceX < 16.0f) return true;  // Sektor Depan
        } else {
            if (distanceX < 4.0f)  return true;  // Sektor Belakang
        }
    } 
    // Deteksi di sela-sela tangga dekat
    else if (playerInStairArea && std::abs(ghostBot.currentFloor - playerFloor) <= 1) {
        return true;
    }
    return false;
}

// =========================================================================
// STATE 1: CHASE MODE (Pengejaran Agresif)
// =========================================================================
void executeChaseMode(int playerFloor) {
    float distanceX = std::abs(playerX - ghostBot.x);
    float distanceZ = std::abs(playerZ - ghostBot.z);

    // KONDISI A: Satu lantai di koridor utama -> Terkam langsung koordinat player
    if (ghostBot.currentFloor == playerFloor && ghostBot.z >= 2.0f && playerZ >= 2.0f) {
        float nextX = ghostBot.x;
        float nextZ = ghostBot.z;

        if (ghostBot.x < playerX)      { nextX += ghostBot.speed; ghostBot.movingRight = true; }
        else if (ghostBot.x > playerX) { nextX -= ghostBot.speed; ghostBot.movingRight = false; }
        if (ghostBot.z < playerZ)       nextZ += ghostBot.speed;
        else if (ghostBot.z > playerZ)  nextZ -= ghostBot.speed;

        if (!checkCollision(nextX, ghostBot.z)) ghostBot.x = nextX;
        if (!checkCollision(ghostBot.x, nextZ)) ghostBot.z = nextZ;
        ghostBot.y = ghostBot.currentFloor * FLOOR_HEIGHT;
    } 
    // KONDISI B: Beda lantai -> Berjalan merapat menuju gerbang tangga terdekat
    else {
        int tanggaNodeX = (ghostBot.currentFloor == 0) ? 1 : ((ghostBot.currentFloor == 1) ? 4 : 7);
        float targetTanggaX = nodes[tanggaNodeX].x;

        if (std::abs(ghostBot.x - targetTanggaX) > 0.1f && ghostBot.z >= 2.0f) {
            float stepX = (ghostBot.x < targetTanggaX) ? ghostBot.speed : -ghostBot.speed;
            if (!checkCollision(ghostBot.x + stepX, ghostBot.z)) {
                ghostBot.x += stepX;
                ghostBot.movingRight = (stepX > 0);
            }
        } 
        else {
            // Meluncur masuk sumbu Z tangga untuk menyusul lantai player
            if (ghostBot.currentFloor < playerFloor) {
                ghostBot.z -= ghostBot.speed * 1.2f;
                if (ghostBot.y >= (ghostBot.currentFloor + 1) * FLOOR_HEIGHT - 0.5f) {
                    ghostBot.z = 2.0f;
                    ghostBot.y = (ghostBot.currentFloor + 1) * FLOOR_HEIGHT;
                }
            } else {
                ghostBot.z -= ghostBot.speed * 1.2f;
                if (ghostBot.y <= (ghostBot.currentFloor - 1) * FLOOR_HEIGHT + 0.5f) {
                    ghostBot.z = 2.0f;
                    ghostBot.y = (ghostBot.currentFloor - 1) * FLOOR_HEIGHT;
                }
            }
        }
    }

    // Pemicu Game Over / Tertangkap
    if (distanceX < 0.8f && distanceZ < 1.0f && ghostBot.currentFloor == playerFloor) {
        playerX = 33.0f; playerY = 0.8f; playerZ = 2.0f; 
        initBot(); 
    }
}

// =========================================================================
// STATE 2: STAIR NAVIGATION (Sistem Gerak Naik-Turun di Dalam Tangga)
// =========================================================================
void executeStairPatrol() {

    ghostBot.x = 33.0f; 
    bool mauNaik = (ghostBot.currentFloor < 2 && ghostBot.movingRight);

    if (mauNaik) {
        if (ghostBot.targetNodeIndex != 100) {
            ghostBot.targetNodeIndex = 99; 
            ghostBot.z -= ghostBot.speed * 1.2f;
            if (ghostBot.z <= -5.3f) { ghostBot.z = -5.3f; ghostBot.targetNodeIndex = 100; }
        } else {
            ghostBot.z += ghostBot.speed * 1.2f;
            float base = ghostBot.currentFloor * FLOOR_HEIGHT;
            float t = (ghostBot.z - (-5.3f)) / 7.3f;
            if (t < 0.0f) t = 0.0f; if (t > 1.0f) t = 1.0f;
            ghostBot.y = base + 2.4f + (t * 1.6f);
        }

        if (ghostBot.z >= 2.0f) {
            int nextFloor = ghostBot.currentFloor + 1;
            ghostBot.z = 2.0f; ghostBot.y = nextFloor * FLOOR_HEIGHT; ghostBot.x = 36.0f;
            ghostBot.movingRight = true;
            ghostBot.targetNodeIndex = nextFloor * 3 + 1; 
        }
    } 
    else {
        if (ghostBot.targetNodeIndex != 100) {
            ghostBot.targetNodeIndex = 99;
            ghostBot.z -= ghostBot.speed * 1.2f;
            float base = ghostBot.currentFloor * FLOOR_HEIGHT;
            float t = (2.0f - ghostBot.z) / 7.3f;
            if (t < 0.0f) t = 0.0f; if (t > 1.0f) t = 1.0f;
            ghostBot.y = base - (t * 1.6f);

            if (ghostBot.z <= -5.3f) { ghostBot.z = -5.3f; ghostBot.targetNodeIndex = 100; }
        } else {
            ghostBot.z += ghostBot.speed * 1.2f;
        }

        if (ghostBot.z >= 2.0f) {
            int nextFloor = ghostBot.currentFloor - 1;
            ghostBot.z = 2.0f; ghostBot.y = nextFloor * FLOOR_HEIGHT; ghostBot.x = 30.0f;
            ghostBot.movingRight = false;
            ghostBot.targetNodeIndex = nextFloor * 3 + 1; 
        }
    }
}

// =========================================================================
// STATE 3: CORRIDOR MOVEMENT (Mondar-mandir di Lorong Utama Lantai Aktif)
// =========================================================================
void executeCorridorPatrol(int baseIndex) {
    Waypoint nodeKiri  = nodes[baseIndex];
    Waypoint nodeKanan = nodes[baseIndex + 2];

    if (ghostBot.movingRight) {
        float stepX = ghostBot.x + ghostBot.speed;
        if (stepX < nodeKanan.x && !checkCollision(stepX, ghostBot.z)) {
            ghostBot.x = stepX;
        } else {
            ghostBot.movingRight = false;
        }
    } else {
        float stepX = ghostBot.x - ghostBot.speed;
        if (stepX > nodeKiri.x && !checkCollision(stepX, ghostBot.z)) {
            ghostBot.x = stepX;
        } else {
            ghostBot.movingRight = true;
        }
    }

    // Saklar Reset targetNodeIndex setelah masa razia Searching selesai
    if (ghostBot.targetNodeIndex != -1) {
        Waypoint currentTarget = nodes[ghostBot.targetNodeIndex];
        if (std::abs(ghostBot.x - currentTarget.x) < 0.4f) {
            if (ghostBot.isSearching) {
                ghostBot.isSearching = false;
                ghostBot.speed = 0.05f; 
            }
            ghostBot.targetNodeIndex = baseIndex + 1; 
        }
    }
}

// =========================================================================
// SYSTEM: PEMICU MASUK TANGGA
// =========================================================================
void checkStairTrigger(int playerFloor, int baseIndex) {
    if (ghostBot.x >= 32.8f && ghostBot.x <= 33.2f) {

        // JIKA sedang memburu player (Chase atau Searching)
        if (ghostBot.isChasing || ghostBot.isSearching) {

            // PERBAIKAN BUG: Hanya belok masuk tangga jika lantai player BENAR-BENAR BERBEDA!
            if (playerFloor > ghostBot.currentFloor && ghostBot.currentFloor < 2) {
                ghostBot.movingRight = true; 
                ghostBot.z = 1.9f; 
            } 
            else if (playerFloor < ghostBot.currentFloor && ghostBot.currentFloor > 0) {
                ghostBot.movingRight = false; 
                ghostBot.z = 1.9f;
            }
            // Jika player satu lantai, bot tetap lurus mengabaikan lubang tangga!
        } 
        // JIKA patroli biasa (Sembunyi aman)
        else {
            // Beri kesempatan 30% saja untuk naik/turun lantai saat patroli biar gak mondar-mandir tangga terus
            if (rand() % 100 < 30) {
                if (ghostBot.movingRight && ghostBot.currentFloor < 2) {
                    ghostBot.z = 1.9f; 
                } 
                else if (!ghostBot.movingRight && ghostBot.currentFloor > 0) {
                    ghostBot.z = 1.9f; 
                }
            }
        }
    }
}

// =========================================================================
// CORE FUNCTION: UPDATE LOOP UTAMA (Sangat Ringkas & Rapi)
// =========================================================================
void updateBot() {
    // Skip bot update during cutscene
    if (cutsceneManager.isRunning()) {
        return;
    }

    int playerFloor = (int)(playerY / FLOOR_HEIGHT);
    ghostBot.currentFloor = (int)(ghostBot.y / FLOOR_HEIGHT);
    ghostBot.dirX = (ghostBot.movingRight) ? 1.0f : -1.0f;

    int baseIndex = ghostBot.currentFloor * 3;

    // PERBAIKAN: Clamp Y agar tidak jatuh ke bawah tanah
    float minY = ghostBot.currentFloor * FLOOR_HEIGHT;
    float maxY = (ghostBot.currentFloor + 1) * FLOOR_HEIGHT;
    if (ghostBot.y < minY) ghostBot.y = minY;
    if (ghostBot.y > maxY + 1.0f) ghostBot.y = maxY;

    // PERBAIKAN: Clamp X agar tidak keluar gedung
    if (ghostBot.x < 1.0f) ghostBot.x = 1.0f;
    if (ghostBot.x > 51.0f) ghostBot.x = 51.0f;

    // PERBAIKAN: Clamp Z
    if (ghostBot.z < -6.6f) ghostBot.z = -6.6f;
    if (ghostBot.z > 4.0f) ghostBot.z = 4.0f;

    // 1. Jalankan sinkronisasi ketinggian tanjakan tangga
    handleStairInterpolation();

    // 2. Scan pandangan mata bot terhadap posisi player
    bool playerSpotted = scanForPlayer();

    // 3. Atur transisi status/state AI
    if (playerSpotted) {
        ghostBot.isChasing = true;
        ghostBot.isSearching = false;
        ghostBot.speed = 0.07f;

        // Play chase sound when spotted
        if (!isChaseSoundPlaying) {
            soundManager.playSound(SOUND_CHASE);
            isChaseSoundPlaying = true;
        }
    } else if (ghostBot.isChasing) {
        ghostBot.isChasing = false;
        ghostBot.isSearching = true; 
        ghostBot.speed = 0.07f;      
        ghostBot.targetNodeIndex = baseIndex + (ghostBot.movingRight ? 2 : 0);

        // Stop chase sound when not chasing anymore
        if (isChaseSoundPlaying) {
            soundManager.stopSound(SOUND_CHASE);
            isChaseSoundPlaying = false;

            // PERBAIKAN KRUSIAL: Restart backsound setelah chase berakhir
            soundManager.restartBackgroundIfNeeded();
        }
    }

    // 4. EKSEKUSI BEHAVIOR BERDASARKAN STATE AKTIF
    if (ghostBot.isChasing) {
        executeChaseMode(playerFloor);
    } 
    else if (ghostBot.z < 2.0f) {
        executeStairPatrol();
    } 
    else {
        executeCorridorPatrol(baseIndex);
        checkStairTrigger(playerFloor, baseIndex);
    }

    // DEBUG: Print posisi bot setiap 60 frame (sekitar 1 detik)
    static int debugCounter = 0;
    if (++debugCounter >= 60) {
        debugCounter = 0;
        printf("[BOT DEBUG] x=%.2f y=%.2f z=%.2f floor=%d chasing=%d\n",
               ghostBot.x, ghostBot.y, ghostBot.z, 
               ghostBot.currentFloor, ghostBot.isChasing ? 1 : 0);
    }
}

void drawBot() {
    glPushMatrix();
        glTranslatef(ghostBot.x, ghostBot.y + 1.0f, ghostBot.z); 
        glEnable(GL_COLOR_MATERIAL); 
        if (ghostBot.isChasing && (int)(glutGet(GLUT_ELAPSED_TIME) / 100) % 2 == 0) glColor3f(1.0f, 1.0f, 0.0f); 
        else glColor3f(0.6f, 0.0f, 0.0f); 
        glPushMatrix();
            glScalef(0.6f, 1.5f, 0.6f);
            glutSolidCube(1.0f);
        glPopMatrix();
        glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();
}
