#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <vector>
#include <cstdlib>

#include "World.h"
#include "Building.h"
#include "Drawing.h"

bool lockedRooms[NUM_FLOORS][NUM_ROOMS_PER_FLOOR] = { {false} };

void registerCollider(float x, float z, float w, float d,
                      float yMin, float yMax) {
    BoundingBox b;
    b.xMin = x;      b.xMax = x + w;
    b.zMin = z - d;  b.zMax = z;
    b.yMin = yMin;   b.yMax = yMax;
    colliders.push_back(b);
}

void handleStairs() {
    bool inX1   = (playerX >= 32.0f && playerX <= 34.0f);
    bool inX2   = (playerX >  34.0f && playerX <= 36.0f);
    bool inXall = (playerX >= 32.0f && playerX <= 36.0f);
    bool inZs   = (playerZ <= -1.0f && playerZ >= -4.6f);
    bool inZb   = (playerZ <  -4.6f && playerZ >= -6.6f);

    bool  inStair = false;
    float targetY = playerY;

    for (int f = 0; f < NUM_FLOORS - 1; f++) {
        float base   = f * FLOOR_HEIGHT;
        float yStart = base + 0.8f;
        float yBord  = base + 3.2f;
        float yEnd   = base + FLOOR_HEIGHT + 0.8f;

        if (inX1 && inZs &&
            playerY >= yStart - 0.4f && playerY <= yBord + 0.4f) {
            float t = (-1.0f - playerZ) / 3.6f;
            targetY = yStart + t * (yBord - yStart);
            inStair = true; break;
        }
        if (inXall && inZb &&
            playerY >= yBord - 0.4f && playerY <= yBord + 0.4f) {
            targetY = yBord;
            inStair = true; break;
        }
        if (f < NUM_FLOORS - 1 &&
            inX2 && inZs &&
            playerY >= yBord - 0.4f && playerY <= yEnd + 0.4f) {
            float t = (playerZ - (-4.6f)) / 3.6f;
            targetY = yBord + t * (yEnd - yBord);
            inStair = true; break;
        }
    }

    if (inStair) {
        playerY = targetY;
    } else {
        if (playerZ > -1.0f) {
            float bestY = 0.8f;
            for (int f = NUM_FLOORS - 1; f >= 0; f--) {
                float fy = f * FLOOR_HEIGHT + 0.8f;
                if (playerY >= fy - 0.3f) { bestY = fy; break; }
            }
            playerY = bestY;
        }
    }
}

void buildPhysicalWorld() {
    colliders.clear();
    for (int f = 0; f < NUM_FLOORS; f++) {
        float yBot = f * FLOOR_HEIGHT;
        float yTop = yBot + FLOOR_HEIGHT;

        registerCollider(0.0f,  3.9f, 52.0f, 0.2f, yBot, yTop);
        registerCollider(0.0f, -10.0f, 52.0f, 0.2f, yBot, yTop);

        // Pintu utama hanya blokir jika tertutup (isDoorOpen = false)
        if (!isDoorOpen) {
            registerCollider(1.0f, 0.0f, 2.0f, 0.2f, yBot, yTop);
        }

        // --- COLLIDER UNTUK RUANGAN TERKUNCI ---
        // Daftar posisi pintu untuk setiap ruangan per lantai
        // Room 0: x=0..8,   door at x=1,  z=0, width=2 (not flipped)
        // Room 1: x=8..16,  door at x=13, z=0, width=2 (flipped, doorX=5 -> 8+5=13)
        // Room 2: x=16..24, door at x=17, z=0, width=2 (not flipped)
        // Room 3: x=24..32, AREA TANGGA - skip
        // Room 4: x=40..48, door at x=41, z=0, width=2 (not flipped)
        // Room 5: x=48..56, door at x=49, z=0, width=2 (not flipped)

        struct DoorInfo {
            float x;
            float z;
            float w;
            float d;
        };

        DoorInfo roomDoors[NUM_ROOMS_PER_FLOOR] = {
            { 1.0f,  0.0f, 2.0f, 0.2f },  // Room 0
            { 13.0f, 0.0f, 2.0f, 0.2f },  // Room 1 (flipped: 8+5=13)
            { 17.0f, 0.0f, 2.0f, 0.2f },  // Room 2
//            { 29.0f,  0.0f, 2.0f, 0.2f },  // Room 3
            { 0.0f,  0.0f, 0.0f, 0.0f },  // Room 4, Tangga
            { 37.0f, 0.0f, 2.0f, 0.2f },  // Room 5
            { 45.0f, 0.0f, 2.0f, 0.2f },  // Room 6
        };

        for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
            if (lockedRooms[f][r]) {
                // Tambahkan collider di pintu ruangan terkunci
                registerCollider(roomDoors[r].x, roomDoors[r].z, 
                                roomDoors[r].w, roomDoors[r].d, yBot, yTop);
            }
        }

        for (int i = 0; i < 4; i++) {
            float sx = i * 8.0f;
            registerCollider(sx, 0.0f, 0.2f, 10.0f, yBot, yTop);
            if (i % 2 == 0) {
                registerCollider(sx,        0.0f, 1.0f, 0.2f, yBot, yTop);
                registerCollider(sx + 3.0f, 0.0f, 5.0f, 0.2f, yBot, yTop);
            } else {
                registerCollider(sx,        0.0f, 5.0f, 0.2f, yBot, yTop);
                registerCollider(sx + 7.0f, 0.0f, 1.0f, 0.2f, yBot, yTop);
            }
        }

        for (int i = 5; i <= 6; i++) {
            float sx = (i - 1) * 8.0f + 4.0f;
            registerCollider(sx, 0.0f, 0.2f, 10.0f, yBot, yTop);
//            if (i == 5) {
//                registerCollider(sx,        0.0f, 1.0f, 0.2f, yBot, yTop);
//                registerCollider(sx + 3.0f, 0.0f, 5.0f, 0.2f, yBot, yTop);
//            } else {
//                registerCollider(sx,        0.0f, 5.0f, 0.2f, yBot, yTop);
//                registerCollider(sx + 7.0f, 0.0f, 1.0f, 0.2f, yBot, yTop);
//            }
            registerCollider(sx,        0.0f, 1.0f, 0.2f, yBot, yTop);
            registerCollider(sx + 3.0f, 0.0f, 5.0f, 0.2f, yBot, yTop);
            
        }
        
        // --- COLLIDER UNTUK MEJA ---
        // Setiap ruangan memiliki 9 meja (3x3 grid)
        // Meja tidak ada di area tangga (room 3)
        float deskSpacingX = (8.0f - 0.4f) / 3.0f;  // 2.5333
        float deskSpacingZ = (10.0f - 2.5f) / 3.0f; // 2.5
        float deskStartX = 0.5f;
        float deskStartZ = -1.8f;
        float deskW = 1.4f;   // lebar meja
        float deskD = 0.7f;   // kedalaman meja
        float deskH = 0.8f;   // tinggi collider meja (dari lantai)
        
        float roomStarts[NUM_ROOMS_PER_FLOOR] = {0.0f, 8.0f, 16.0f, 24.0f, 36.0f, 44.0f};
        
        for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
            
            float roomBaseX = roomStarts[r];
            
            for (int col = 0; col < 3; col++) {
                for (int row = 0; row < 3; row++) {
                    float deskX = roomBaseX + deskStartX + col * deskSpacingX;
                    float deskZ = deskStartZ - row * deskSpacingZ;
                    
                    // Register collider untuk meja
                    registerCollider(deskX, deskZ, deskW, deskD, yBot, yBot + deskH);
                }
            }
        }
    }
}

void randomizeLockedRooms() {
    // Reset semua ruangan ke tidak terkunci
    for (int f = 0; f < NUM_FLOORS; f++) {
        for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
            lockedRooms[f][r] = false;
        }
    }

    // Untuk setiap lantai, random 1-2 ruangan yang terkunci
    // Hindari area tangga (ruangan index 3)
    for (int f = 0; f < NUM_FLOORS; f++) {
        int numLocked = 1 + (rand() % 2); // 1 atau 2 ruangan terkunci per lantai
        int lockedCount = 0;
        int attempts = 0;
        
        while (lockedCount < numLocked && attempts < 100) {
            int room = rand() % NUM_ROOMS_PER_FLOOR;
            // Skip area tangga (index 3) dan ruangan yang sudah terkunci
            if (room != 3 && !lockedRooms[f][room]) {
                lockedRooms[f][room] = true;
                lockedCount++;
            }
            attempts++;
        }
    }

    for (int floor = 0; floor < NUM_FLOORS; floor++) {
        for (int room = 0; room < NUM_ROOMS_PER_FLOOR; room++) {
            if (lockedRooms[floor][room]) {
            printf("[LOCKED] floor=%d room=%d\n",
                   floor,
                   room);
         }
        }
    }
}

bool checkCollision(float nextX, float nextZ) {
    for (size_t i = 0; i < colliders.size(); i++) {
        const BoundingBox& b = colliders[i];
        if (playerY + 1.6f < b.yMin || playerY > b.yMax) continue;
        if (nextX + pRadius > b.xMin && nextX - pRadius < b.xMax &&
            nextZ + pRadius > b.zMin && nextZ - pRadius < b.zMax)
            return true;
    }
    return false;
}
