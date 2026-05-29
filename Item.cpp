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
struct SpawnZone {
    float xMin, xMax, zMin, zMax;
    bool  isCorridor;
    int   roomIndex; 
};

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

static const SpawnZone ALL_ZONES[] = {
    // Koridor utama
    { 6.0f,  50.0f,  1.5f,  3.5f,  true,  -1 },

    // Ruangan kiri
    { 2.0f,   6.0f, -7.0f, -2.5f, false,  0 },   // Room 0: x=0..8
    {10.0f,  14.0f, -7.0f, -2.5f, false,  1 },   // Room 1: x=8..16
    {18.0f,  22.0f, -7.0f, -2.5f, false,  2 },   // Room 2: x=16..24

    // Ruangan kanan
    {42.0f,  46.0f, -7.0f, -2.5f, false,  4 },   // Room 4: x=40..48
    {50.0f,  53.0f, -7.0f, -2.5f, false,  5 },   // Room 5: x=48..56
};
static const int NUM_ALL_ZONES = 6;
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



static const SpawnZone SPAWN_ZONES[] = {
    // Koridor utama (lebih aman, dalam area playable)
    { 6.0f,  50.0f,  1.5f,  3.5f,  true  },

    // Ruangan kiri
    { 2.0f,   6.0f, -7.0f, -2.5f, false },
    {10.0f,  14.0f, -7.0f, -2.5f, false },
    {18.0f,  22.0f, -7.0f, -2.5f, false },

    // Ruangan kanan
    {42.0f,  46.0f, -7.0f, -2.5f, false },
    {50.0f,  53.0f, -7.0f, -2.5f, false },
};
static const int NUM_SPAWN_ZONES = sizeof(SPAWN_ZONES) / sizeof(SPAWN_ZONES[0]);

// Random float dalam range [lo, hi]
static float randf(float lo, float hi) {
    return lo + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (hi - lo);
}

// Cek apakah zona spawn valid (tidak di ruangan terkunci)
static bool isZoneValid(const SpawnZone& zone, int floor) {
    if (zone.isCorridor) return true; // Koridor selalu valid
    if (zone.roomIndex < 0) return true;
    if (zone.roomIndex >= NUM_ROOMS_PER_FLOOR) return true;
    // Cek apakah ruangan terkunci di lantai ini
    return !lockedRooms[floor][zone.roomIndex];
}

static WorldItem spawnItem(ItemType type, int preferredFloor) {
    WorldItem item;
    item.type       = type;
    item.collected  = false;
    item.bobTimer   = randf(0.0f, 6.28f);
    item.rotAngle   = randf(0.0f, 360.0f);
    item.floor      = preferredFloor;

    // Filter zona yang tersedia untuk lantai ini
    vector<SpawnZone> validZones;
    
    for (int i = 0; i < NUM_ALL_ZONES; i++) {
        if (isZoneValid(ALL_ZONES[i], preferredFloor)) {
            validZones.push_back(ALL_ZONES[i]);
        }
    }

    // Jika tidak ada zona valid (semua ruangan terkunci), fallback ke koridor
    if (validZones.empty()) {
        validZones.push_back(ALL_ZONES[0]); // Koridor
    }

    // Pilih zona spawn random dari yang valid
    int  zoneIdx = rand() % validZones.size();
    const SpawnZone& zone = validZones[zoneIdx];

    item.x = randf(zone.xMin, zone.xMax);
    item.z = randf(zone.zMin, zone.zMax);

    // Safety margin (sama seperti sebelumnya)
    if (item.x < 1.5f) item.x = 1.5f;
    if (item.x > 54.5f) item.x = 54.5f;
    if (item.z > 3.5f) item.z = 3.5f;
    if (item.z < -7.5f) item.z = -7.5f;

    item.y = preferredFloor * FLOOR_HEIGHT + ITEM_BASE_HEIGHT;

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
    for (int t = 0; t < NUM_ITEM_TYPES; t++) {
        g_inventory[t] = 0;
    }

    g_pickupMsg[0]  = '\0';
    g_pickupMsgTimer = 0.0f;

    int totalItems = 0;
    for (int t = 0; t < NUM_ITEM_TYPES; t++) {
        totalItems += SPAWN_COUNTS[t];
    }

    // Spawn tiap tipe item
    for (int t = 0; t < NUM_ITEM_TYPES; t++) {
        for (int n = 0; n < SPAWN_COUNTS[t]; n++) {
            // Sebar merata di berbagai lantai
            int floor = n % NUM_FLOORS;
            // Untuk keycard: selalu di lantai teratas agar lebih menantang
            if (t == ITEM_KEYCARD) floor = NUM_FLOORS - 1;
            // Untuk senter: lantai dasar agar mudah ditemukan duluan
            if (t == ITEM_FLASHLIGHT) floor = 0;

            g_items.push_back(spawnItem((ItemType)t, floor));
        }
    }
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

        glPushMatrix();
            glTranslatef(item.x, item.y + bobOffset, item.z);
            glRotatef(item.rotAngle, 0.0f, 1.0f, 0.0f);

            // Gambar aura di bawah item
            drawItemAura(item.type, item.bobTimer);

            // Set warna dan gambar bentuk item
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

