#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib> 
#include <cmath>    
#include "bot.h"
#include "World.h" 

EnemyBot ghostBot;

// Daftar 11 Node Utama Gedung untuk Navigasi 
const int NUM_NODES = 11;
Waypoint nodes[NUM_NODES] = {
    { 2.0f,  0.0f, 2.0f },   // Node 0: Lantai 1 - Ujung Kiri
    { 33.0f, 0.0f, 2.0f },  // Node 1: Lantai 1 - Depan Tangga (Tengah)
    { 54.0f, 0.0f, 2.0f },  // Node 2: Lantai 1 - Ujung Kanan
    
    { 2.0f,  4.0f, 2.0f },   // Node 3: Lantai 2 - Ujung Kiri
    { 33.0f, 4.0f, 2.0f },  // Node 4: Lantai 2 - Depan Tangga (Tengah)
    { 54.0f, 4.0f, 2.0f },  // Node 5: Lantai 2 - Ujung Kanan
    
    { 2.0f,  8.0f, 2.0f },   // Node 6: Lantai 3 - Ujung Kiri
    { 33.0f, 8.0f, 2.0f },  // Node 7: Lantai 3 - Depan Tangga (Tengah)
    { 54.0f, 8.0f, 2.0f },  // Node 8: Lantai 3 - Ujung Kanan
    
    { 33.0f, 2.4f, -5.3f }, // Node 9: Bordes Tangga Bawah (Lantai 1->2)
    { 33.0f, 6.4f, -5.3f }  // Node 10: Bordes Tangga Atas (Lantai 2->3)
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
    ghostBot.targetNodeIndex = 1; // Target awal Node Tengah Lantai 1
}

void updateBot() {
    int playerFloor = (int)(playerY / FLOOR_HEIGHT);
    ghostBot.currentFloor = (int)(ghostBot.y / FLOOR_HEIGHT);
    
    ghostBot.dirX = (ghostBot.movingRight) ? 1.0f : -1.0f;

    float distanceX = std::abs(playerX - ghostBot.x);
    float distanceZ = std::abs(playerZ - ghostBot.z);

    int baseIndex = ghostBot.currentFloor * 3; 

    // --- INTERPOLASI VERTIKAL DI TANGGA ---
    if (ghostBot.x >= 32.0f && ghostBot.x <= 36.0f && ghostBot.z < 2.0f) {
        float base = ghostBot.currentFloor * FLOOR_HEIGHT;
        if (ghostBot.z <= -1.0f && ghostBot.z >= -4.6f) {
            float t = (-1.0f - ghostBot.z) / 3.6f;
            if (t < 0.0f) t = 0.0f; if (t > 1.0f) t = 1.0f;
            ghostBot.y = base + t * 2.4f; 
        } else if (ghostBot.z < -4.6f && ghostBot.z >= -6.6f) {
            ghostBot.y = base + 2.4f;      
        }
    }

    bool playerInStairArea = (playerZ <= 0.5f && playerX >= 31.0f && playerX <= 37.0f);

    // SCANNING VISION & CHASE TRIGGER ---
    bool canSeePlayer = false;

    if (ghostBot.currentFloor == playerFloor && ghostBot.z >= 2.0f && playerZ > 0.5f) {
        float toPlayerX = playerX - ghostBot.x;
        float dotProduct = ghostBot.dirX * toPlayerX;

        if (dotProduct > 0.0f) {
            if (distanceX < 16.0f) canSeePlayer = true; // Sektor Depan
        } else {
            if (distanceX < 4.0f)  canSeePlayer = true; // Sektor Belakang
        }
    } 
    else if (playerInStairArea && std::abs(ghostBot.currentFloor - playerFloor) <= 1) {
        canSeePlayer = true;
    }

    // TRANSISI STATUS AI
    if (canSeePlayer) {
        ghostBot.isChasing = true;
        ghostBot.isSearching = false;
        ghostBot.speed = 0.07f; 
    } else {
        if (ghostBot.isChasing) {
            ghostBot.isChasing = false;
            ghostBot.isSearching = true; // Aktifkan razia ujung koridor
            ghostBot.speed = 0.07f;      // Tetap lari kencang!

            // Kunci target node ujung berdasarkan lantai aktif saat ini secara mutlak!
            if (ghostBot.movingRight) {
                ghostBot.targetNodeIndex = baseIndex + 2; // Node Ujung Kanan (2, 5, atau 8)
            } else {
                ghostBot.targetNodeIndex = baseIndex;     // Node Ujung Kiri (0, 3, atau 6)
            }
        }
    }

    // CHASE MODE
    if (ghostBot.isChasing) {
        // KONDISI A: Satu lantai di koridor utama
        if (ghostBot.currentFloor == playerFloor && ghostBot.z >= 2.0f && playerZ >= 2.0f) {
            float nextX = ghostBot.x;
            float nextZ = ghostBot.z;

            if (ghostBot.x < playerX) { nextX += ghostBot.speed; ghostBot.movingRight = true; }
            else if (ghostBot.x > playerX) { nextX -= ghostBot.speed; ghostBot.movingRight = false; }
            if (ghostBot.z < playerZ)       nextZ += ghostBot.speed;
            else if (ghostBot.z > playerZ)  nextZ -= ghostBot.speed;

            if (!checkCollision(nextX, ghostBot.z)) ghostBot.x = nextX;
            if (!checkCollision(ghostBot.x, nextZ)) ghostBot.z = nextZ;
            ghostBot.y = ghostBot.currentFloor * FLOOR_HEIGHT;
        } 
        // KONDISI B: Beda lantai 
        else {
            // Tentukan node gerbang tangga tujuan sementara tanpa meng-override targetNodeIndex utama jika tidak perlu
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
                // Proses meluncur masuk sumbu Z tangga
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

        // Trigger Ketangkap
        if (distanceX < 0.8f && distanceZ < 1.0f && ghostBot.currentFloor == playerFloor) {
            playerX = 33.0f; playerY = 0.8f; playerZ = 2.0f; 
            initBot(); 
        }
        return; 
    }

    //  PATROLI NORMAL / SEARCHING DI KORIDOR 
    if (ghostBot.z < 2.0f) {
        ghostBot.x = 33.0f; 
        
        // Cek apakah bot mau naik atau turun saat berada di dalam tangga
        bool mauNaik = (ghostBot.currentFloor < 2 && ghostBot.movingRight);

        if (mauNaik) {
            if (ghostBot.targetNodeIndex != 100) {
                ghostBot.targetNodeIndex = 99; // State mengunci arah mundur
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
                ghostBot.targetNodeIndex = nextFloor * 3 + 1; // Kembalikan ke node tengah lantai baru
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
                ghostBot.targetNodeIndex = nextFloor * 3 + 1; // Kembalikan ke node tengah lantai baru
            }
        }
        return;
    }

    // Pergerakan horizontal koridor utama menggunakan batas node dinamis per lantai
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

    // Mengecek apakah bot sudah sampai di node sasaran (Ujung kiri/kanan/tengah)
    if (ghostBot.targetNodeIndex != -1) {
        Waypoint currentTarget = nodes[ghostBot.targetNodeIndex];
        if (std::abs(ghostBot.x - currentTarget.x) < 0.4f) {
            if (ghostBot.isSearching) {
                ghostBot.isSearching = false;
                ghostBot.speed = 0.05f; // Selesai menyelidik, turunkan kecepatan
            }
            ghostBot.targetNodeIndex = baseIndex + 1; // Setel ulang target ke node tengah lantai aktif
        }
    }

    // PEMICU MASUK TANGGA 
    if (ghostBot.x >= 32.8f && ghostBot.x <= 33.2f) {
        if (ghostBot.isChasing || ghostBot.isSearching) {
            if (playerFloor > ghostBot.currentFloor && ghostBot.currentFloor < 2) {
                ghostBot.movingRight = true; ghostBot.z = 1.9f; 
            } 
            else if (playerFloor < ghostBot.currentFloor && ghostBot.currentFloor > 0) {
                ghostBot.movingRight = false; ghostBot.z = 1.9f;
            }
        } 
        else {
            // Patroli santai biasa jika player sembunyi dengan aman
            if (ghostBot.movingRight && ghostBot.currentFloor < 2) {
                ghostBot.z = 1.9f; 
            } 
            else if (!ghostBot.movingRight && ghostBot.currentFloor > 0) {
                ghostBot.z = 1.9f; 
            }
        }
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
