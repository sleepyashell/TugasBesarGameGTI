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

#ifndef NUM_ROOMS_PER_FLOOR
#define NUM_ROOMS_PER_FLOOR 6
#endif

#ifndef NUM_FLOORS
#define NUM_FLOORS 3
#endif

#ifndef FLOOR_HEIGHT
#define FLOOR_HEIGHT 5.0f
#endif

bool lockedRooms[NUM_FLOORS][NUM_ROOMS_PER_FLOOR] = { {false} };
extern bool isDoorOpen;

// === POSISI PINTU ABSOLUT GLOBAL ===
// Pintu ada di Z=0, menghadap ke dalam ruangan (ke -Z)
DoorInfo doorInfos[NUM_ROOMS_PER_FLOOR] = {
    // Room 0: x=0..8
    { 1.0f,  0.0f, 2.0f, 0.2f, false, 0.0f,  8.0f },
    // Room 1: x=8..16
    { 13.0f, 0.0f, 2.0f, 0.2f, true,  8.0f,  8.0f },
    // Room 2: x=16..24
    { 17.0f, 0.0f, 2.0f, 0.2f, false, 16.0f, 8.0f },
    // Room 3: x=24..32, AREA TANGGA - no door
    { 0.0f,  0.0f, 0.0f, 0.0f, false, 24.0f, 4.0f },  // ? lebar 4, bukan 8
    // Room 4: x=36..44  ? digeser dari 40..48
    { 37.0f, 0.0f, 2.0f, 0.2f, false, 36.0f, 8.0f },
    // Room 5: x=44..52  ? digeser dari 48..56
    { 45.0f, 0.0f, 2.0f, 0.2f, false, 44.0f, 8.0f },
};

int getRoomIndexFromX(float x) {
    if (x >= 0.0f  && x < 8.0f)  return 0;
    if (x >= 8.0f  && x < 16.0f) return 1;
    if (x >= 16.0f && x < 24.0f) return 2;
    if (x >= 24.0f && x < 36.0f) return 3;  // tangga 4 unit + buffer
    if (x >= 36.0f && x < 44.0f) return 4;  // ? dari 40..48 jadi 36..44
    if (x >= 44.0f && x < 52.0f) return 5;  // ? dari 48..56 jadi 44..52
    return -1;
}

// === DATA ANIMASI PINTU ===
DoorAnimation doorAnims[NUM_FLOORS][NUM_ROOMS_PER_FLOOR];

void initDoorAnimations() {
    for (int f = 0; f < NUM_FLOORS; f++) {
        for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
            doorAnims[f][r].isOpen = false;
            doorAnims[f][r].isAnimating = false;
            doorAnims[f][r].currentAngle = 0.0f;
            doorAnims[f][r].targetAngle = 0.0f;
            doorAnims[f][r].animSpeed = 5.0f;  // 5 derajat/frame
            doorAnims[f][r].roomIndex = r;
            doorAnims[f][r].floor = f;
        }
    }
}

bool updateDoorAnimations(float dt) {
    bool anyAnimating = false;
    for (int f = 0; f < NUM_FLOORS; f++) {
        for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
            DoorAnimation& door = doorAnims[f][r];
            if (!door.isAnimating) continue;
            
            anyAnimating = true;
            float diff = door.targetAngle - door.currentAngle;
            
            if (std::abs(diff) < 0.5f) {
                door.currentAngle = door.targetAngle;
                door.isAnimating = false;
                door.isOpen = (door.targetAngle > 45.0f);
            } else {
                float step = door.animSpeed * dt * 60.0f;
                if (diff > 0) {
                    door.currentAngle += step;
                    if (door.currentAngle > door.targetAngle) 
                        door.currentAngle = door.targetAngle;
                } else {
                    door.currentAngle -= step;
                    if (door.currentAngle < door.targetAngle) 
                        door.currentAngle = door.targetAngle;
                }
            }
        }
    }
    
    if (anyAnimating) {
        buildPhysicalWorld(); 
    }
    
    return anyAnimating;
}

void triggerDoorOpen(int floor, int roomIndex) {
    if (floor < 0 || floor >= NUM_FLOORS) return;
    if (roomIndex < 0 || roomIndex >= NUM_ROOMS_PER_FLOOR) return;
    
    DoorAnimation& door = doorAnims[floor][roomIndex];
    if (!door.isOpen && !door.isAnimating) {
        door.isAnimating = true;
        door.targetAngle = 85.0f;
    }
}

void triggerDoorClose(int floor, int roomIndex) {
    if (floor < 0 || floor >= NUM_FLOORS) return;
    if (roomIndex < 0 || roomIndex >= NUM_ROOMS_PER_FLOOR) return;
    
    DoorAnimation& door = doorAnims[floor][roomIndex];
    if (door.isOpen && !door.isAnimating) {
        door.isAnimating = true;
        door.targetAngle = 0.0f;
    }
}

// === CEK PLAYER DI DEPAN PINTU ===
bool isPlayerInFrontOfDoor(int floor, int roomIndex) {
    if (roomIndex < 0 || roomIndex >= NUM_ROOMS_PER_FLOOR) return false;
    if (roomIndex == 3) return false; // Area tangga
    
    DoorInfo& door = doorInfos[roomIndex];
    
    // Player harus di koridor (depan pintu, Z positif), bukan dalam ruangan
    bool inFrontZ = (playerZ >= -1.5f && playerZ <= 4.0f);
    // Player dekat dengan sumbu X pintu (lebar pintu + margin)
    bool nearDoorX = (playerX >= door.x - 1.5f && playerX <= door.x + door.w + 1.5f);
    // Sama lantai
    bool sameFloor = ((int)(playerY / FLOOR_HEIGHT) == floor);
    
    return inFrontZ && nearDoorX && sameFloor;
}

// === CEK PROXIMITY PINTU ===
void checkDoorProximity() {
    int playerFloor = (int)(playerY / FLOOR_HEIGHT);
    if (playerFloor < 0 || playerFloor >= NUM_FLOORS) return;
    
    for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
        if (r == 3) continue; // Skip area tangga
        if (lockedRooms[playerFloor][r]) continue; // Ruangan terkunci
        
        DoorInfo& door = doorInfos[r];
        float doorCenterX = door.x + door.w / 2.0f;
        float doorCenterZ = door.z - door.d / 2.0f;
        
        // Jarak Euclidean 2D
        float dx = playerX - doorCenterX;
        float dz = playerZ - doorCenterZ;
        float distance = sqrtf(dx*dx + dz*dz);
        
        // Cek apakah player benar-benar di depan pintu
        bool playerInFront = isPlayerInFrontOfDoor(playerFloor, r);
        
        // Buka jika dekat DAN di depan
        if (distance < DOOR_OPEN_DISTANCE && playerInFront) {
            triggerDoorOpen(playerFloor, r);
        } 
        // Tutup jika menjauh ATAU sudah masuk ruangan
        else if (distance > DOOR_CLOSE_DISTANCE) {
            triggerDoorClose(playerFloor, r);
        }
    }
}

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
        
        for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
            if (r == 3) {
                // Area tangga: dinding penuh dari x=24 ke x=32
                registerCollider(24.0f, 0.0f, 8.0f, 0.2f, yBot, yTop);
                continue;
            }
            
            DoorInfo& door = doorInfos[r];
            float rs = door.roomStartX;
            float rw = door.roomWidth;
            
            // Dinding kiri pintu
            if (door.x > rs) {
                registerCollider(rs, 0.0f, door.x - rs, 0.2f, yBot, yTop);
            }
            
            // Dinding kanan pintu
            float doorRight = door.x + door.w;
            if (doorRight < rs + rw) {
                registerCollider(doorRight, 0.0f, (rs + rw) - doorRight, 0.2f, yBot, yTop);
            }
            
            // Dinding atas pintu (lintel)
            registerCollider(door.x, 2.5f, door.w, 0.2f, yBot + 2.5f, yTop);
        }

        // ========================================
        // 4. PINTU UTAMA (hanya lantai 1)
        // ========================================
        if (f == 0 && !isDoorOpen) {
            registerCollider(1.0f, 0.0f, 2.0f, 0.2f, yBot, yTop);
        }
        // ========================================
        // 5. COLLIDER PINTU RUANGAN (DINAMIS)
        // ========================================
        for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
            if (r == 3) continue; // Skip area tangga
            
            DoorInfo& door = doorInfos[r];
            
            if (lockedRooms[f][r]) {
                // RUANGAN TERKUNCI - collider penuh
                registerCollider(door.x, door.z, door.w, door.d, yBot, yTop);
            } else {
                // RUANGAN TIDAK TERKUNCI - collider dinamis
                float angle = doorAnims[f][r].currentAngle;
                
                if (angle < 20.0f) {
                    // Hampir tertutup - collider penuh
                    registerCollider(door.x, door.z, door.w, door.d, yBot, yTop);
                } else if (angle < 60.0f) {
                    // Semi terbuka - collider sisa di engsel
                    float remainingWidth = door.w * 0.25f;
                    if (door.flipped) {
                        // Engsel kanan, sisa di kanan
                        registerCollider(door.x + door.w - remainingWidth, door.z, 
                                        remainingWidth, door.d, yBot, yTop);
                    } else {
                        // Engsel kiri, sisa di kiri
                        registerCollider(door.x, door.z, 
                                        remainingWidth, door.d, yBot, yTop);
                    }
                }
            }
        }

        // ========================================
        // 6. SEKAT ANTAR RUANGAN (DINDING TEGAK)
        // ========================================
        float sekatX[] = { 7.95f, 15.95f, 23.95f, 31.95f, 35.95f, 43.95f };
        for (int i = 0; i < 6; i++) {
            registerCollider(sekatX[i], 0.0f, 0.05f, 10.0f, yBot, yTop);
        }

        // ========================================
        // 7. DINDING LUAR KIRI & KANAN
        // ========================================
        registerCollider(0.0f, 0.0f, 0.1f, 10.0f, yBot, yTop);   // Kiri
        registerCollider(51.9f, 0.0f, 0.1f, 10.0f, yBot, yTop);   // Kanan
        // Pojok kiri koridor
		registerCollider(52.0f, 4.0f, 0.2f, 4.0f, yBot, yTop);
		// Pojok kanan koridor
        if (f > 0){
        	registerCollider(0.0f, 4.0f, 0.2f, 4.0f, yBot, yTop);
        }
        // ========================================
        // 8. COLLIDER MEJA
        // ========================================
        float deskSpacingX = (8.0f - 0.4f) / 3.0f;
        float deskSpacingZ = (10.0f - 2.5f) / 3.0f;
        float deskStartX = 0.5f;
        float deskStartZ = -1.8f;
        float deskW = 1.4f;
        float deskD = 0.7f;
        float deskH = 0.8f;
        
        float roomStarts[NUM_ROOMS_PER_FLOOR] = {0.0f, 8.0f, 16.0f, 24.0f, 36.0f, 44.0f};
        
        for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
            if (r == 3) continue; // Skip area tangga
            
            float roomBaseX = roomStarts[r];
            
            for (int col = 0; col < 3; col++) {
                for (int row = 0; row < 3; row++) {
                    float deskX = roomBaseX + deskStartX + col * deskSpacingX;
                    float deskZ = deskStartZ - row * deskSpacingZ;
                    
                    registerCollider(deskX, deskZ, deskW, deskD, yBot, yBot + deskH);
                }
            }
        }

        // ========================================
        // 9. COLLIDER TANGGA
        // ========================================
        if (f < NUM_FLOORS - 1) {
            registerCollider(32.0f, -6.0f, 4.0f, 0.2f, yBot, yTop);
            registerCollider(34.0f, -1.0f, 0.05f, 3.5f, yBot, yTop);
            if (f == 0){
            	registerCollider(32.0f, -4.5f, 2.0f, 0.05f, 0.8f, 2.0f);
            }

        }
    }
}

void randomizeLockedRooms() {
    for (int f = 0; f < NUM_FLOORS; f++) {
        for (int r = 0; r < NUM_ROOMS_PER_FLOOR; r++) {
            lockedRooms[f][r] = false;
        }
    }
    
    
    
    

    for (int f = 0; f < NUM_FLOORS; f++) {
        int numLocked = 1 + (rand() % 2);
        int lockedCount = 0;
        int attempts = 0;
        
        while (lockedCount < numLocked && attempts < 100) {
            int room = rand() % NUM_ROOMS_PER_FLOOR;
            if (room != 3 && !lockedRooms[f][room]) {
                lockedRooms[f][room] = true;
                lockedCount++;
            }
            attempts++;
        }
    }
    
    initDoorAnimations();
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
