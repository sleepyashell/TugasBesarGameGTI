#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib> 
#include <cmath>    
#include "bot.h"
#include "World.h" // Membaca checkCollision

// Instansiasi array hantu global untuk 3 lantai
EnemyBot ghostBots[3];

// =========================================================================
// DATA ROUTING: WAYPOINTS PATROLI LORONG UTAMA (CUMA 3 NODE, ANTI-REDUNDAN)
// =========================================================================
const int NUM_NODES = 3;
Waypoint nodes[NUM_NODES] = {
    { 2.0f  },  // Node 0: Ujung Kiri
    { 33.0f },  // Node 1: Tengah Lorong (Depan Tangga)
    { 50.0f }   // Node 2: Ujung Kanan
};

// =========================================================================
// INITIALIZATION (Melahirkan Hantu Stabil di Setiap Lantai)
// =========================================================================
void initBot() {
    for (int i = 0; i < 3; i++) {
        // Hantu Lantai 2 (i == 1) bertelur di Ujung Kiri (Node 0), jalan ke Kanan
        if (i == 1) {	
            ghostBots[i].x = nodes[0].x;
            ghostBots[i].dirX = 1.0f;
            ghostBots[i].movingRight = true;
            ghostBots[i].targetNodeIndex = 1; // Menuju tengah
        } 
        // Hantu Lantai 1 & 3 bertelur di Ujung Kanan (Node 2), jalan ke Kiri
        else {
            ghostBots[i].x = nodes[2].x;
            ghostBots[i].dirX = -1.0f;
            ghostBots[i].movingRight = false;
            ghostBots[i].targetNodeIndex = 1; // Menuju tengah
        }  
        
        ghostBots[i].y = (i * 5) + 0.8f; // Tinggi lantai asli kelompokmu
        ghostBots[i].z = 2.0f;
        ghostBots[i].speed = 0.05f;  
        ghostBots[i].currentFloor = i;
        ghostBots[i].isChasing = false;
        ghostBots[i].isSearching = false;
    }
}

// =========================================================================
// MODULE 1: SCANNING VISION ARRAY (Hanya deteksi player di koridor lantai sama)
// =========================================================================
bool scanForPlayer(int botIdx) {
    // Kunci pembagian tinggi ubin lantai menggunakan angka 5.0f konstan
    int playerFloor = (int)(playerY / 5.0f); 

    // KUNCI STEALTH: Hantu hanya mendeteksi jika satu lantai dan player di koridor terbuka (playerZ >= 0.0f)
    if (ghostBots[botIdx].currentFloor == playerFloor && playerZ >= 0.0f) {
        float distanceX = std::abs(playerX - ghostBots[botIdx].x);
        float toPlayerX = playerX - ghostBots[botIdx].x;
        float dotProduct = ghostBots[botIdx].dirX * toPlayerX;

        if (dotProduct > 0.0f) {
            if (distanceX < 5.0f) return true;  // Sektor pandangan depan
        } else {
            if (distanceX < 2.5f)  return true;  // Sektor pendengaran belakang
        }
    } 
    return false;
}

// =========================================================================
// MODULE 2: CORRIDOR PATROL ARRAY (Ronda Mandiri di Koridor Utama)
// =========================================================================
void executeCorridorPatrol(int botIdx) {
    Waypoint nodeKiri  = nodes[0];
    Waypoint nodeKanan = nodes[2];

    // AUTO-RETURN ASLI: Mundur dulu ke arah ubin koridor utama (Z = 2.0f) jika sehabis chase
    if (ghostBots[botIdx].z < 2.0f) {
        ghostBots[botIdx].z += ghostBots[botIdx].speed;
        if (ghostBots[botIdx].z > 2.0f) ghostBots[botIdx].z = 2.0f;
        return;
    }

    if (ghostBots[botIdx].movingRight) {
        float stepX = ghostBots[botIdx].x + ghostBots[botIdx].speed;
        if (stepX < nodeKanan.x && !checkCollision(stepX, ghostBots[botIdx].z)) {
            ghostBots[botIdx].x = stepX;
        } else {
            ghostBots[botIdx].movingRight = false;
        }
    } else {
        float stepX = ghostBots[botIdx].x - ghostBots[botIdx].speed;
        if (stepX > nodeKiri.x && !checkCollision(stepX, ghostBots[botIdx].z)) {
            ghostBots[botIdx].x = stepX;
        } else {
            ghostBots[botIdx].movingRight = true;
        }
    }
}

// =========================================================================
// MODULE 3: DIRECT CHASE STATE ARRAY (Anti-Nyangkut Masuk & Keluar Kelas)
// =========================================================================
void executeChaseMode(int botIdx, int playerFloor) {
    float distanceX = std::abs(playerX - ghostBots[botIdx].x);
    float distanceZ = std::abs(playerZ - ghostBots[botIdx].z);

    if (ghostBots[botIdx].currentFloor == playerFloor) {
        float nextX = ghostBots[botIdx].x;
        float nextZ = ghostBots[botIdx].z;
        
        // Hitung indeks ruangan berdasarkan posisi player saat ini
        int roomIndex = (int)(playerX / 8.0f);
        
        // JIKA target player di dalam kelas, tapi hantu ke-i posisinya di luar ruangan
        if (roomIndex != 1 && roomIndex != 3 && roomIndex >= 4) {
            // Biar fleksibel, kalau bot di dalam kelas tapi player di luar, roomIndex-nya pakai posisi bot sendiri
            if (ghostBots[botIdx].z < 0.0f && playerZ >= 0.0f) {
                roomIndex = (int)(ghostBots[botIdx].x / 8.0f);
            }
        }

        // Ambil titik koordinat pintu asli sesuai struktur gedung kelompokmu
        float targetDoorX = roomIndex * 8.0f + 2.0f;
        if (roomIndex == 1 || roomIndex == 3) {
            targetDoorX = roomIndex * 8.0f + 6.0f;
        }
        if (roomIndex >= 4) {
            float actualX = (roomIndex) * 8.0f + 4.0f;
            targetDoorX = actualX + 2.0f;
        }

        // ----------------------------------------------------------------=
        // KONDISI A: PROSES MASUK (Player di dalam kelas [Z < 0], Bot di koridor [Z >= 0])
        // ----------------------------------------------------------------=
        if (playerZ < 0.0f && ghostBots[botIdx].z >= 0.0f) {
            // GAYA KAKU: Selesaikan meluruskan sumbu X dulu sampai pas di pintu
            if (std::abs(ghostBots[botIdx].x - targetDoorX) > 0.1f) {
                if (ghostBots[botIdx].x < targetDoorX)      { nextX += ghostBots[botIdx].speed; ghostBots[botIdx].movingRight = true; }
                else if (ghostBots[botIdx].x > targetDoorX) { nextX -= ghostBots[botIdx].speed; ghostBots[botIdx].movingRight = false; }
            } 
            // Setelah X benar-benar sejajar pintu, baru menusuk lurus masuk sumbu Z kelas
            else if (ghostBots[botIdx].z > 0.0f) {
                nextZ -= ghostBots[botIdx].speed;
            }
        }
        // ----------------------------------------------------------------=
        // KONDISI B: PROSES KELUAR (Player sudah di koridor [Z >= 0], Bot masih di kelas [Z < 0])
        // ----------------------------------------------------------------=
        else if (playerZ >= 0.0f && ghostBots[botIdx].z < 0.0f) {
            // GAYA KAKU: Kunci posisi X hantu di dalam kelas agar berjalan lurus ke arah lubang pintu keluar dahulu
            if (std::abs(ghostBots[botIdx].x - targetDoorX) > 0.1f) {
                if (ghostBots[botIdx].x < targetDoorX)      { nextX += ghostBots[botIdx].speed; ghostBots[botIdx].movingRight = true; }
                else if (ghostBots[botIdx].x > targetDoorX) { nextX -= ghostBots[botIdx].speed; ghostBots[botIdx].movingRight = false; }
            }
            // Setelah sejajar dengan lubang pintu keluar, baru dorong sumbu Z maju lurus ke koridor utama
            else if (ghostBots[botIdx].z < 2.0f) {
                nextZ += ghostBots[botIdx].speed;
            }
        }
        // ----------------------------------------------------------------=
        // KONDISI C: SAMA-SAMA DI KORIDOR / SAMA-SAMA DI DALAM KELAS
        // ----------------------------------------------------------------=
        else {
            if (ghostBots[botIdx].x < playerX)      { nextX += ghostBots[botIdx].speed; ghostBots[botIdx].movingRight = true; }
            else if (ghostBots[botIdx].x > playerX) { nextX -= ghostBots[botIdx].speed; ghostBots[botIdx].movingRight = false; }
            
            if (ghostBots[botIdx].z < playerZ)       nextZ += ghostBots[botIdx].speed;
            else if (ghostBots[botIdx].z > playerZ)  nextZ -= ghostBots[botIdx].speed;
        }

        // Cek benturan rintangan global
        if (!checkCollision(nextX, ghostBots[botIdx].z)) ghostBots[botIdx].x = nextX;
        if (!checkCollision(ghostBots[botIdx].x, nextZ)) ghostBots[botIdx].z = nextZ;
    }

    // Pemicu Game Over / Reset total
    if (distanceX < 0.8f && distanceZ < 1.0f && ghostBots[botIdx].currentFloor == playerFloor) {
        playerX = 33.0f; playerY = 0.8f; playerZ = 2.0f;
        initBot(); // Reset posisi ketiga hantu kembali tertib ke pos asalnya masing-masing
    }
}

// =========================================================================
// CORE FUNCTION: LOOP UPDATE UTAMA (PROSES PARAREL 3 LANTAI)
// =========================================================================
void updateBot() {
    // Kunci pendeteksi lantai dasar player konstan dibagi 5.0f
    int playerFloor = (int)(playerY / 5.0f); 

    // Looping untuk memproses state mesin masing-masing hantu L1, L2, dan L3
    for (int i = 0; i < 3; i++) {
        ghostBots[i].dirX = (ghostBots[i].movingRight) ? 1.0f : -1.0f;
        
        float dX = playerX - ghostBots[i].x;
        float dZ = playerZ - ghostBots[i].z;
        float totalDistance = sqrtf(dX * dX + dZ * dZ);

        if (ghostBots[i].isChasing) {
            int playerFloorNow = (int)(playerY / 5.0f); 
            
            // PROTEKSI AREA TANGGA ASLI: 
            // Titik tengah tangga berada di poros X = 32..36, dan ubinnya masuk ke sumbu Z negatif (< 0.8f)
            bool playerInStairArea = (playerX >= 32.0f && playerX <= 36.0f && playerZ < 0.8f);

            // STOP CONDITION A & C: Player ganti lantai ATAU masuk area tangga -> Hantu lepas kendali
            if (ghostBots[i].currentFloor != playerFloorNow || playerInStairArea) {
                ghostBots[i].isChasing = false;
                ghostBots[i].isSearching = false; // Langsung matikan agar bot langsung berjalan mundur ke koridor
                ghostBots[i].speed = 0.05f;
            }
            // STOP CONDITION B ASLI: Jarak player kabur sembunyi melebihi batas 8.0f unit
            else if (totalDistance > 8.0f) {
                ghostBots[i].isChasing = false;    // <<< HANTU BERHENTI NGEJAR!
                ghostBots[i].isSearching = false;
                ghostBots[i].speed = 0.05f;
            }
        } 
        else {
            bool playerSpotted = scanForPlayer(i);

            if (playerSpotted) {
                ghostBots[i].isChasing = true;
                ghostBots[i].isSearching = false;
                ghostBots[i].speed = 0.065f; // Lari memburu player
            } else if (ghostBots[i].isSearching) {
                ghostBots[i].isSearching = false;
            }
        }

        // Eksekusi pergerakan hantu lantai i berdasarkan state-nya
        if (ghostBots[i].isChasing) {
            executeChaseMode(i, playerFloor);
        } 
        else {
            executeCorridorPatrol(i);
        }
    }
}

// =========================================================================
// RENDER GRAPHIC ARRAY
// =========================================================================
void drawBot() {
    glEnable(GL_COLOR_MATERIAL);
    
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
            glTranslatef(ghostBots[i].x, ghostBots[i].y, ghostBots[i].z);
            
            if (ghostBots[i].isChasing && (int)(glutGet(GLUT_ELAPSED_TIME) / 100) % 2 == 0) {
                glColor3f(1.0f, 1.0f, 0.0f);
            } else { 
                glColor3f(0.5f, 0.0f, 0.0f);
            }
            
            glPushMatrix();
                glScalef(0.6f, 1.5f, 0.6f);
                glutSolidCube(1.0f);
            glPopMatrix();
        glPopMatrix();
    }
    
    glDisable(GL_COLOR_MATERIAL);
}

