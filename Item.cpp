#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <iostream>

#include "Item.h"
#include "World.h"    // playerX, playerY, playerZ, NUM_FLOORS, FLOOR_HEIGHT
#include "Drawing.h"  // drawBlock
#include "Material.h" // setMaterial

using namespace std;
// ==========================================
// KONSTANTA ITEM
// ==========================================

static const float PICKUP_RADIUS     = 1.2f;   // jarak maks pengambilan item
static const float BOB_AMPLITUDE     = 0.12f;  // amplitudo animasi naik-turun
static const float BOB_SPEED         = 2.0f;   // kecepatan bobbing 
static const float ROT_SPEED         = 90.0f;  // kecepatan rotasi
static const float ITEM_BASE_HEIGHT  = 0.9f;   // tinggi default item di atas lantai

// ==========================================
// DATA ITEM (nama, deskripsi, wajib/tidak)
// ==========================================

static const ItemInfo ITEM_CATALOG[NUM_ITEM_TYPES] = {
    { ITEM_KEYCARD,    "Key Card",    "Kartu akses untuk membuka pintu keluar",   true  },
    { ITEM_FLASHLIGHT, "Senter",      "Penerangan darurat. Jangan sampai mati",   true  },
    { ITEM_DOCUMENT,   "Dokumen",     "Bukti yang tidak boleh tertinggal",         true  },
    { ITEM_MEDKIT,     "P3K",         "Semoga tidak perlu dipakai",                false },
    { ITEM_BATTERY,    "Baterai",     "Cadangan daya untuk senter",                false },
};

// Berapa item required yang harus ada di dunia
static const int REQUIRED_COUNTS[NUM_ITEM_TYPES] = {
    1,  // KEYCARD   - 1 buah
    1,  // FLASHLIGHT- 1 buah
    3,  // DOCUMENT  - 3 buah (tersebar di berbagai lantai)
    0,  // MEDKIT    - tidak required, tapi bisa di-spawn
    0,  // BATTERY   - tidak required
};

// Berapa total item (required + bonus) per tipe yang di-spawn
static const int SPAWN_COUNTS[NUM_ITEM_TYPES] = {
    1,  // KEYCARD
    1,  // FLASHLIGHT
    3,  // DOCUMENT
    2,  // MEDKIT    - bonus
    2,  // BATTERY   - bonus
};

// ==========================================
// STATE GLOBAL
// ==========================================

static vector<WorldItem>        g_items;
static int                      g_inventory[NUM_ITEM_TYPES] = { 0 };
static bool                     g_initialized = false;

// Pesan pickup yang tampil sebentar di layar
static char     g_pickupMsg[64]    = "";
static float    g_pickupMsgTimer   = 0.0f;
static const float PICKUP_MSG_DURATION = 2.5f;

// ==========================================
// WARNA ITEM
// ==========================================

static void setItemColor(ItemType type) {
    switch (type) {
        case ITEM_KEYCARD:
            // Kuning terang - mudah terlihat
            setMaterial(0.95f, 0.85f, 0.10f, 1.0f,
                        0.80f, 0.70f, 0.10f, 64.0f);
            break;
        case ITEM_FLASHLIGHT:
            // Abu metalik terang
            setMaterial(0.70f, 0.72f, 0.75f, 1.0f,
                        0.90f, 0.90f, 0.90f, 96.0f);
            break;
        case ITEM_DOCUMENT:
            // Putih kekuningan (kertas tua)
            setMaterial(0.88f, 0.82f, 0.65f, 1.0f,
                        0.40f, 0.38f, 0.30f, 16.0f);
            break;
        case ITEM_MEDKIT:
            // Merah menyala
            setMaterial(0.85f, 0.10f, 0.10f, 1.0f,
                        0.60f, 0.05f, 0.05f, 32.0f);
            break;
        case ITEM_BATTERY:
            // Hijau fosfor
            setMaterial(0.15f, 0.90f, 0.20f, 1.0f,
                        0.10f, 0.70f, 0.15f, 48.0f);
            break;
        default:
            setMaterial(1.0f, 1.0f, 1.0f, 1.0f);
    }
}


// Helper: render string pakai glutBitmapCharacter di posisi HUD (koordinat ortho 0-100)
static void hudPrint(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
}

// HUD
void drawItemHUD()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // ==========================
    // POSISI HUD ITEM - pojok kanan atas
    // (tidak menabrak koordinat player di kiri atas)
    // ==========================
    const float startX  = 88.0f;   // kanan layar
    const float startY  = 95.0f;   // baris paling atas
    const float lineGap =  4.0f;

    char buffer[128];

    // --- Judul OBJECTIVES ---
    glColor3f(1.0f, 1.0f, 0.2f);
    snprintf(buffer, sizeof(buffer), "OBJECTIVES %d/%d",
            getCollectedRequiredCount(),
            getTotalRequiredCount());
    hudPrint(startX, startY, buffer);

    // --- Key Card ---
    bool haveKeycard = (g_inventory[ITEM_KEYCARD] >= REQUIRED_COUNTS[ITEM_KEYCARD]);
    glColor3f(haveKeycard ? 0.3f : 0.9f,
              haveKeycard ? 1.0f : 0.9f,
              haveKeycard ? 0.3f : 0.2f);
    snprintf(buffer, sizeof(buffer), "[%s] Key Card  : %d/%d",
            haveKeycard ? "v" : " ",
            g_inventory[ITEM_KEYCARD],
            REQUIRED_COUNTS[ITEM_KEYCARD]);
    hudPrint(startX, startY - lineGap, buffer);

    // --- Flashlight ---
    bool haveFlash = (g_inventory[ITEM_FLASHLIGHT] >= REQUIRED_COUNTS[ITEM_FLASHLIGHT]);
    glColor3f(haveFlash ? 0.3f : 0.9f,
              haveFlash ? 1.0f : 0.9f,
              haveFlash ? 0.3f : 0.2f);
    snprintf(buffer, sizeof(buffer), "[%s] Flashlight: %d/%d",
            haveFlash ? "v" : " ",
            g_inventory[ITEM_FLASHLIGHT],
            REQUIRED_COUNTS[ITEM_FLASHLIGHT]);
    hudPrint(startX, startY - lineGap * 2, buffer);

    // --- Document ---
    bool haveDoc = (g_inventory[ITEM_DOCUMENT] >= REQUIRED_COUNTS[ITEM_DOCUMENT]);
    glColor3f(haveDoc ? 0.3f : 0.9f,
              haveDoc ? 1.0f : 0.9f,
              haveDoc ? 0.3f : 0.2f);
    snprintf(buffer, sizeof(buffer), "[%s] Document  : %d/%d",
            haveDoc ? "v" : " ",
            g_inventory[ITEM_DOCUMENT],
            REQUIRED_COUNTS[ITEM_DOCUMENT]);
    hudPrint(startX, startY - lineGap * 3, buffer);

    // --- Pickup notification (tengah layar, di bawah) ---
    if (g_pickupMsgTimer > 0.0f) {
        glColor3f(0.3f, 1.0f, 0.3f);
        hudPrint(38.0f, 20.0f, g_pickupMsg);
    }

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ==========================================
// RENDER BENTUK ITEM
// ==========================================

void drawCylinder(float radius, float height, int slices) {
    const float PI = 3.1415926f;
    float step = 2.0f * PI / slices;

    // Body cylinder
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float angle = i * step;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        glNormal3f(cos(angle), 0.0f, sin(angle));

        glVertex3f(x, 0.0f, z);
        glVertex3f(x, height, z);
    }
    glEnd();

    // Tutup bawah
    glBegin(GL_POLYGON);
    for (int i = 0; i < slices; i++) {
        float angle = i * step;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        glVertex3f(x, 0.0f, z);
    }
    glEnd();

    // Tutup atas
    glBegin(GL_POLYGON);
    for (int i = 0; i < slices; i++) {
        float angle = i * step;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        glVertex3f(x, height, z);
    }
    glEnd();
}

void drawDisk(float innerRadius, float outerRadius, int slices) {
    const float PI = 3.1415926f;
    float step = 2.0f * PI / slices;

    glBegin(GL_TRIANGLE_STRIP);

    for (int i = 0; i <= slices; i++) {
        float angle = i * step;

        float c = cos(angle);
        float s = sin(angle);

        glVertex3f(c * innerRadius, 0.0f, s * innerRadius);
        glVertex3f(c * outerRadius, 0.0f, s * outerRadius);
    }

    glEnd();
}

static void drawItemShape(ItemType type) {
    switch (type) {
        case ITEM_KEYCARD:
            // Kartu tipis persegi panjang
            drawBlock(0.25f, 0.38f, 0.03f);
            // Chip kecil di tengah
            glPushMatrix();
                glTranslatef(0.07f, 0.18f, -0.01f);
                setMaterial(0.60f, 0.55f, 0.20f, 1.0f, 0.90f, 0.85f, 0.30f, 128.0f);
                drawBlock(0.10f, 0.08f, 0.02f);
            glPopMatrix();
            break;

        case ITEM_FLASHLIGHT:
            // Body senter
            glPushMatrix();
                glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
                drawCylinder(0.05f, 0.32f, 24);
            glPopMatrix();

            // Kepala senter (nempel)
            glPushMatrix();
                glTranslatef(0.12f, 0.0f, 0.0f);
                glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
                drawCylinder(0.075f, 0.12f, 24);
            glPopMatrix();
            break;

        case ITEM_DOCUMENT:
            // Lembaran kertas tipis + lipatan
            drawBlock(0.30f, 0.40f, 0.02f);
            // Garis teks (visual)
            glPushMatrix();
                glTranslatef(0.04f, 0.30f, -0.005f);
                setMaterial(0.30f, 0.25f, 0.15f, 1.0f);
                drawBlock(0.22f, 0.025f, 0.015f);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0.04f, 0.22f, -0.005f);
                setMaterial(0.30f, 0.25f, 0.15f, 1.0f);
                drawBlock(0.18f, 0.025f, 0.015f);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0.04f, 0.14f, -0.005f);
                setMaterial(0.30f, 0.25f, 0.15f, 1.0f);
                drawBlock(0.20f, 0.025f, 0.015f);
            glPopMatrix();
            break;

        case ITEM_MEDKIT:
            // Kotak P3K
            drawBlock(0.28f, 0.22f, 0.14f);
            // Palang merah
            glPushMatrix();
                glTranslatef(0.09f, 0.08f, -0.005f);
                setMaterial(0.95f, 0.95f, 0.95f, 1.0f);
                drawBlock(0.10f, 0.06f, 0.02f);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0.12f, 0.05f, -0.005f);
                setMaterial(0.95f, 0.95f, 0.95f, 1.0f);
                drawBlock(0.04f, 0.12f, 0.02f);
            glPopMatrix();
            break;

        case ITEM_BATTERY:
            // Tabung baterai AA
            glPushMatrix();
                drawCylinder(0.05f, 0.28f, 12);
            glPopMatrix();

            // Terminal positif
            glPushMatrix();
                glTranslatef(0.0f, 0.28f, 0.0f);
                setMaterial(0.80f, 0.80f, 0.80f, 1.0f,
                            1.0f, 1.0f, 1.0f, 128.0f);
                drawCylinder(0.025f, 0.04f, 12);
            glPopMatrix();
            break;

        default:
            drawBlock(0.2f, 0.2f, 0.2f);
    }
}

// ==========================================
// AURA / GLOW ITEM (ring berkedip di bawah)
// ==========================================

static void drawItemAura(ItemType type, float bobOffset) {
    bool isRequired = ITEM_CATALOG[type].isRequired;

    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);

    float pulse = 0.5f + 0.5f * sinf(bobOffset * BOB_SPEED * 1.5f);

    if (isRequired) {
        // Cincin kuning-emas untuk item wajib
        glColor4f(1.0f, 0.85f, 0.0f, 0.18f + 0.12f * pulse);
    } else {
        // Cincin biru untuk item bonus
        glColor4f(0.3f, 0.6f, 1.0f, 0.14f + 0.10f * pulse);
    }

    // Disk tipis di bawah item
    glPushMatrix();
        glTranslatef(0.12f, 0.05f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f + 0.1f * pulse, 0.5f + 0.1f * pulse, 1.0f);
        drawDisk(0.15f, 0.45f, 16);
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}

// ==========================================
// SPAWN LOGIC
// ==========================================

// 6 ruangan per lantai sesuai layout Building.cpp:
// Room 0..3: X=0..8, 8..16, 16..24, 24..32
// Room 4..5: X=40..48, 48..56  (skip area tangga X=32..40)
// Depth ruangan: Z=0..Z=-10, margin 0.8f dari setiap dinding

static const int ROOMS_PER_FLOOR = 6;

struct RoomZone {
    float xMin, xMax, zMin, zMax;
};

static const RoomZone ROOM_ZONES[ROOMS_PER_FLOOR] = {
    {  0.8f,  7.2f, -9.0f, -1.0f },  // Room 0: X=0..8, Z=-10..-0 dengan margin
    {  8.8f, 15.2f, -9.0f, -1.0f },  // Room 1: X=8..16
    { 16.8f, 23.2f, -9.0f, -1.0f },  // Room 2: X=16..24
    { 24.8f, 31.2f, -9.0f, -1.0f },  // Room 3: X=24..32
    { 40.8f, 47.2f, -9.0f, -1.0f },  // Room 4: X=40..48
    { 48.8f, 50.2f, -9.0f, -1.0f },  // Room 5: X=48..56
};

// Random float dalam range [lo, hi]
static float randf(float lo, float hi) {
    return lo + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (hi - lo);
}

// Spawn 1 item di ruangan dan lantai tertentu
static WorldItem spawnItemInRoom(ItemType type, int floor, int roomIndex) {
    WorldItem item;
    item.type      = type;
    item.collected = false;
    item.bobTimer  = randf(0.0f, 6.28f);
    item.rotAngle  = randf(0.0f, 360.0f);
    item.floor     = floor;

    // Gunakan 4.0f untuk Y position - ini adalah actual visual floor height dari Building.cpp
    // (FLOOR_HEIGHT = 5.0f adalah world spacing, bukan building height)
    const RoomZone& room = ROOM_ZONES[roomIndex];
    item.x = randf(room.xMin, room.xMax);
    item.z = randf(room.zMin, room.zMax);
    item.y = floor * 4.0f + ITEM_BASE_HEIGHT;

    printf("[DEBUG] Item spawn: type=%d, floor=%d, room=%d -> X=%.1f Y=%.1f Z=%.1f\n",
           type, floor, roomIndex, item.x, item.y, item.z);

    return item;
}

// ==========================================
// IMPLEMENTASI API PUBLIK
// ==========================================

void initItems() {
    if (!g_initialized) {
        srand((unsigned int)time(NULL));
        g_initialized = true;
    }

    g_items.clear();
    for (int t = 0; t < NUM_ITEM_TYPES; t++) g_inventory[t] = 0;
    g_pickupMsg[0]   = '\0';
    g_pickupMsgTimer = 0.0f;

    // Total slot: 6 ruangan x 3 lantai = 18 slot
    // Total item di-spawn: 1+1+3+2+2 = 9, jauh < 18 -> aman max 1 per ruangan
    const int TOTAL_ROOMS = ROOMS_PER_FLOOR * NUM_FLOORS; // 18

    // Acak urutan semua slot (Fisher-Yates) agar posisi item bervariasi tiap reset
    int slots[18];
    for (int i = 0; i < TOTAL_ROOMS; i++) slots[i] = i;
    for (int i = TOTAL_ROOMS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = slots[i]; slots[i] = slots[j]; slots[j] = tmp;
    }

    // Kumpulkan request spawn beserta constraint lantai
    struct SpawnRequest { ItemType type; int preferredFloor; };
    SpawnRequest requests[32];
    int reqCount = 0;
    for (int t = 0; t < NUM_ITEM_TYPES; t++) {
        for (int n = 0; n < SPAWN_COUNTS[t]; n++) {
            SpawnRequest sr;
            sr.type = (ItemType)t;
            sr.preferredFloor = -1;
            if (t == ITEM_KEYCARD)    sr.preferredFloor = NUM_FLOORS - 1;
            if (t == ITEM_FLASHLIGHT) sr.preferredFloor = 0;
            if (t == ITEM_DOCUMENT)   sr.preferredFloor = n % NUM_FLOORS;
            requests[reqCount++] = sr;
            printf("[Item] Request %d: type=%d, preferredFloor=%d\n", reqCount-1, sr.type, sr.preferredFloor);
        }
    }
    printf("[Item] Total %d spawn requests\n", reqCount);

    // Assign tiap request ke slot yang belum terpakai (max 1 item per ruangan)
    bool slotUsed[18] = { false };
    for (int r = 0; r < reqCount; r++) {
        int assignedSlot = -1;

        // Cari slot di lantai yang diinginkan dulu
        if (requests[r].preferredFloor >= 0) {
            int pf = requests[r].preferredFloor;
            for (int i = 0; i < TOTAL_ROOMS; i++) {
                if (!slotUsed[i] && (slots[i] / ROOMS_PER_FLOOR) == pf) {
                    assignedSlot = i;
                    break;
                }
            }
        }

        // Fallback: slot kosong mana saja
        if (assignedSlot == -1) {
            for (int i = 0; i < TOTAL_ROOMS; i++) {
                if (!slotUsed[i]) {
                    assignedSlot = i;
                    break;
                }
            }
        }

        if (assignedSlot == -1) {
            printf("[Item] WARNING: tidak ada slot kosong untuk item %d!\n", requests[r].type);
            continue;
        }

        slotUsed[assignedSlot] = true;
        int floor     = slots[assignedSlot] / ROOMS_PER_FLOOR;
        int roomIndex = slots[assignedSlot] % ROOMS_PER_FLOOR;

        printf("[Item] DEBUG: assignedSlot=%d, slots[assignedSlot]=%d, floor=%d, room=%d\n",
               assignedSlot, slots[assignedSlot], floor, roomIndex);
        
        g_items.push_back(spawnItemInRoom(requests[r].type, floor, roomIndex));
        printf("[Item] Spawn type=%d -> lantai %d, ruangan %d\n",
               requests[r].type, floor, roomIndex);
    }

    printf("[Item] Total %d items (max 1 per ruangan, 18 slot tersedia).\n",
           (int)g_items.size());
}

void updateItems(float dt) {
    for (size_t i = 0; i < g_items.size(); i++) {
        if (g_items[i].collected) continue;
        g_items[i].bobTimer  += dt;
        g_items[i].rotAngle  += ROT_SPEED * dt;
        if (g_items[i].rotAngle >= 360.0f) g_items[i].rotAngle -= 360.0f;
    }
    if (g_pickupMsgTimer > 0.0f) {
        g_pickupMsgTimer -= dt;
    }
}

void checkItemPickup() {
    for (size_t i = 0; i < g_items.size(); i++) {
        WorldItem& item = g_items[i];
        if (item.collected) continue;

        float dx = playerX - item.x;
        float dy = playerY - item.y;
        float dz = playerZ - item.z;
        float dist = sqrtf(dx*dx + dy*dy + dz*dz);

        if (dist <= PICKUP_RADIUS) {
            item.collected = true;
            g_inventory[item.type]++;

            // Set pesan pickup
            snprintf(g_pickupMsg, sizeof(g_pickupMsg),
                     "+ %s", ITEM_CATALOG[item.type].name.c_str());
            g_pickupMsgTimer = PICKUP_MSG_DURATION;

            printf("[Item] Picked up: %s | Inventory[%d] = %d\n",
                   ITEM_CATALOG[item.type].name.c_str(),
                   item.type,
                   g_inventory[item.type]);
        }
    }
}

void drawItems() {
    for (size_t i = 0; i < g_items.size(); i++) {
        const WorldItem& item = g_items[i];
        if (item.collected) continue;

        float bobOffset = sinf(item.bobTimer * BOB_SPEED) * BOB_AMPLITUDE;

        // Aura di matrix sendiri, tidak kena rotasi Y item
        glPushMatrix();
            glTranslatef(item.x, item.y + bobOffset, item.z);
            drawItemAura(item.type, item.bobTimer);
        glPopMatrix();

        // Item shape dengan rotasi dan scale
        glPushMatrix();
            glTranslatef(item.x, item.y + bobOffset, item.z);
            glRotatef(item.rotAngle, 0.0f, 1.0f, 0.0f);
            glScalef(1.5f, 1.5f, 1.5f);
            setItemColor(item.type);
            drawItemShape(item.type);
        glPopMatrix();
    }
}

// ==========================================
// QUERY FUNCTIONS
// ==========================================

int getCollectedRequiredCount() {
    int count = 0;
    for (int t = 0; t < NUM_ITEM_TYPES; t++) {
        if (!ITEM_CATALOG[t].isRequired) continue;
        int have = g_inventory[t];
        int need = REQUIRED_COUNTS[t];
        if (have >= need) {
            count += need;
        } else {
            count += have;
        }
    }
    return count;
}

int getTotalRequiredCount() {
    int total = 0;
    for (int t = 0; t < NUM_ITEM_TYPES; t++) {
        if (ITEM_CATALOG[t].isRequired) {
            total += REQUIRED_COUNTS[t];
        }
    }
    return total;
}

bool allRequiredItemsCollected() {
    for (int t = 0; t < NUM_ITEM_TYPES; t++) {
        if (!ITEM_CATALOG[t].isRequired) continue;
        if (g_inventory[t] < REQUIRED_COUNTS[t]) return false;
    }
    return true;
}

void resetItems() {
    g_items.clear();
    for (int t = 0; t < NUM_ITEM_TYPES; t++) {
        g_inventory[t] = 0;
    }
    g_pickupMsg[0]   = '\0';
    g_pickupMsgTimer = 0.0f;
    initItems();
}