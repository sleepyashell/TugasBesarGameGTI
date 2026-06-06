#include "Item.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <iostream>


#include "World.h" 
#include "Drawing.h" 
#include "Material.h" 

using namespace std;




static const float PICKUP_RADIUS     = 1.2f;   
static const float HOVER_AMPLITUDE     = 0.12f;  
static const float HOVER_SPEED         = 2.0f;   
static const float ROT_SPEED         = 90.0f;  
static const float ITEM_BASE_HEIGHT  = 1.2f;   





static const ItemInfo ITEM_CATALOG[NUM_ITEM_TYPES] = {
    { ITEM_KEYCARD,    "Key Card",    "Kartu akses untuk membuka pintu keluar",   true  },
    { ITEM_FLASHLIGHT, "Senter",      "Penerangan darurat. Jangan sampai mati",   true  },
    { ITEM_DOCUMENT,   "Dokumen",     "Bukti yang tidak boleh tertinggal",         true  },
    { ITEM_MEDKIT,     "P3K",         "Semoga tidak perlu dipakai",                false },
    { ITEM_BATTERY,    "Baterai",     "Cadangan daya untuk senter",                false },
};


static const int REQUIRED_COUNTS[NUM_ITEM_TYPES] = {
    1,  
    1,  
    3,  
    0,  
    0,  
};


static const int SPAWN_COUNTS[NUM_ITEM_TYPES] = {
    1,  
    1,  
    3,  
    2,  
    2,  
};





static vector<WorldItem>        g_items;
static int                      g_inventory[NUM_ITEM_TYPES] = { 0 };
static bool                     g_initialized = false;


static char     g_pickupMsg[64]    = "";
static float    g_pickupMsgTimer   = 0.0f;
static const float PICKUP_MSG_DURATION = 2.5f;





static void setItemColor(ItemType type) {
    switch (type) {
        case ITEM_KEYCARD:
            
            setMaterial(0.95f, 0.85f, 0.10f, 1.0f,
                        0.80f, 0.70f, 0.10f, 64.0f);
            break;
        case ITEM_FLASHLIGHT:
            
            setMaterial(0.70f, 0.72f, 0.75f, 1.0f,
                        0.90f, 0.90f, 0.90f, 96.0f);
            break;
        case ITEM_DOCUMENT:
            
            setMaterial(0.88f, 0.82f, 0.65f, 1.0f,
                        0.40f, 0.38f, 0.30f, 16.0f);
            break;
        case ITEM_MEDKIT:
            
            setMaterial(0.85f, 0.10f, 0.10f, 1.0f,
                        0.60f, 0.05f, 0.05f, 32.0f);
            break;
        case ITEM_BATTERY:
            
            setMaterial(0.15f, 0.90f, 0.20f, 1.0f,
                        0.10f, 0.70f, 0.15f, 48.0f);
            break;
        default:
            setMaterial(1.0f, 1.0f, 1.0f, 1.0f);
    }
}



static void hudPrint(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
}


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

    

    
    
    
    
    const float startX  = 88.0f;   
    const float startY  = 95.0f;   
    const float lineGap =  4.0f;

    char buffer[128];


    if (allRequiredItemsCollected()){
    glColor3f(0.2f, 1.0f, 0.2f);
    
    glRasterPos2f(startX, startY - 16.0f);
    const char* msg = "RETURN TO THE GATE";
    while (*msg) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *msg);
        msg++;
    	}
	}
    
    
    glColor3f(1.0f, 1.0f, 0.2f);
    snprintf(buffer, sizeof(buffer), "OBJECTIVES %d/%d",
            getCollectedRequiredCount(),
            getTotalRequiredCount());
    hudPrint(startX, startY, buffer);

    
    bool haveKeycard = (g_inventory[ITEM_KEYCARD] >= REQUIRED_COUNTS[ITEM_KEYCARD]);
    glColor3f(haveKeycard ? 0.3f : 0.9f,
              haveKeycard ? 1.0f : 0.9f,
              haveKeycard ? 0.3f : 0.2f);
    snprintf(buffer, sizeof(buffer), "[%s] Key Card  : %d/%d",
            haveKeycard ? "v" : " ",
            g_inventory[ITEM_KEYCARD],
            REQUIRED_COUNTS[ITEM_KEYCARD]);
    hudPrint(startX, startY - lineGap, buffer);

    
    bool haveFlash = (g_inventory[ITEM_FLASHLIGHT] >= REQUIRED_COUNTS[ITEM_FLASHLIGHT]);
    glColor3f(haveFlash ? 0.3f : 0.9f,
              haveFlash ? 1.0f : 0.9f,
              haveFlash ? 0.3f : 0.2f);
    snprintf(buffer, sizeof(buffer), "[%s] Senter: %d/%d",
            haveFlash ? "v" : " ",
            g_inventory[ITEM_FLASHLIGHT],
            REQUIRED_COUNTS[ITEM_FLASHLIGHT]);
    hudPrint(startX, startY - lineGap * 2, buffer);

    
    bool haveDoc = (g_inventory[ITEM_DOCUMENT] >= REQUIRED_COUNTS[ITEM_DOCUMENT]);
    glColor3f(haveDoc ? 0.3f : 0.9f,
              haveDoc ? 1.0f : 0.9f,
              haveDoc ? 0.3f : 0.2f);
    snprintf(buffer, sizeof(buffer), "[%s] Dokumen  : %d/%d",
            haveDoc ? "v" : " ",
            g_inventory[ITEM_DOCUMENT],
            REQUIRED_COUNTS[ITEM_DOCUMENT]);
    hudPrint(startX, startY - lineGap * 3, buffer);

    
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





void drawCylinder(float radius, float height, int slices) {
    const float PI = 3.1415926f;
    float step = 2.0f * PI / slices;

    
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

    
    glBegin(GL_POLYGON);
    for (int i = 0; i < slices; i++) {
        float angle = i * step;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        glVertex3f(x, 0.0f, z);
    }
    glEnd();

    
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
            
            drawBlock(0.25f, 0.38f, 0.03f);
            
            glPushMatrix();
                glTranslatef(0.07f, 0.18f, -0.01f);
                setMaterial(0.60f, 0.55f, 0.20f, 1.0f, 0.90f, 0.85f, 0.30f, 128.0f);
                drawBlock(0.10f, 0.08f, 0.02f);
            glPopMatrix();
            break;

        case ITEM_FLASHLIGHT:
            
            glPushMatrix();
                glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
                drawCylinder(0.05f, 0.32f, 24);
            glPopMatrix();

            
            glPushMatrix();
                glTranslatef(0.12f, 0.0f, 0.0f);
                glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
                drawCylinder(0.075f, 0.12f, 24);
            glPopMatrix();
            break;

        case ITEM_DOCUMENT:
            
            drawBlock(0.30f, 0.40f, 0.02f);
            
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
            
            drawBlock(0.28f, 0.22f, 0.14f);
            
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
            
            glPushMatrix();
                drawCylinder(0.05f, 0.28f, 12);
            glPopMatrix();

            
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





static void drawItemAura(ItemType type, float bobOffset) {
    bool isRequired = ITEM_CATALOG[type].isRequired;

    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);

    float pulse = 0.5f + 0.5f * sinf(bobOffset * HOVER_SPEED * 1.5f);

    if (isRequired) {
        
        glColor4f(1.0f, 0.85f, 0.0f, 0.18f + 0.12f * pulse);
    } else {
        
        glColor4f(0.3f, 0.6f, 1.0f, 0.14f + 0.10f * pulse);
    }

    
    glPushMatrix();
        glTranslatef(0.12f, 0.05f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f + 0.1f * pulse, 0.5f + 0.1f * pulse, 1.0f);
        drawDisk(0.15f, 0.45f, 16);
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}











struct RoomZone {
    float xMin, xMax, zMin, zMax;
};

static const RoomZone ROOM_ZONES[NUM_ROOMS_PER_FLOOR] = {
    {  1.5f,  6.5f, -8.5f, -2.0f },  
    {  9.5f, 14.5f, -8.5f, -2.0f },  
    { 17.5f, 22.5f, -8.5f, -2.0f },  

    
    { 25.5f, 30.5f, -8.5f, -2.0f },  

    { 37.5f, 42.5f, -8.5f, -2.0f },  
    { 45.5f, 50.5f, -8.5f, -2.0f },  
};


static float randf(float lo, float hi) {
    return lo + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (hi - lo);
}


static WorldItem spawnItemInRoom(ItemType type, int floor, int roomIndex) {
    WorldItem item;
    item.type      = type;
    item.collected = false;
    item.bobTimer  = randf(0.0f, 6.28f);
    item.rotAngle  = randf(0.0f, 360.0f);
    item.floor     = floor;

    const RoomZone& room = ROOM_ZONES[roomIndex];
    
    item.x = randf(room.xMin, room.xMax);
    item.z = randf(room.zMin, room.zMax);

    
    if (item.z > -2.5f)
        item.z = -2.5f;

    
    item.y = floor * FLOOR_HEIGHT + ITEM_BASE_HEIGHT;

    printf("[DEBUG] Item spawn: type=%d, floor=%d, room=%d -> X=%.1f Y=%.1f Z=%.1f\n",
           type, floor, roomIndex, item.x, item.y, item.z);

    return item;
}






void initItems() {
    if (!g_initialized) {
        srand((unsigned int)time(NULL));
        g_initialized = true;
    }

    g_items.clear();

    for (int t = 0; t < NUM_ITEM_TYPES; t++)
        g_inventory[t] = 0;

    g_pickupMsg[0] = '\0';
    g_pickupMsgTimer = 0.0f;

    int slots[NUM_FLOORS * NUM_ROOMS_PER_FLOOR];
    int availableSlots = 0;

    for (int floor = 0; floor < NUM_FLOORS; floor++) {
        for (int room = 0; room < NUM_ROOMS_PER_FLOOR; room++) {

            if (room == 3)
                continue;

            if (lockedRooms[floor][room])
                continue;

            slots[availableSlots++] =
                floor * NUM_ROOMS_PER_FLOOR + room;
        }
    }

    printf("[Item] Available rooms: %d", availableSlots);

    const int REQUIRED_ITEM_COUNT = 5;

    if (availableSlots < REQUIRED_ITEM_COUNT) {
        printf("[Item] ERROR: Tidak cukup ruangan terbuka!");
        return;
    }

    for (int i = availableSlots - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        int tmp = slots[i];
        slots[i] = slots[j];
        slots[j] = tmp;
    }

    bool slotUsed[NUM_FLOORS * NUM_ROOMS_PER_FLOOR] = { false };

    struct SpawnRequest {
        ItemType type;
        int preferredFloor;
    };

    SpawnRequest requests[32];
    int reqCount = 0;

    requests[reqCount++] = { ITEM_KEYCARD, NUM_FLOORS - 1 };
    requests[reqCount++] = { ITEM_FLASHLIGHT, 0 };

    for (int i = 0; i < 3; i++) {
        requests[reqCount++] = { ITEM_DOCUMENT, i % NUM_FLOORS };
    }

    for (int r = 0; r < reqCount; r++) {

        int assignedSlot = -1;

        
        for (int i = 0; i < availableSlots; i++) {

            int floor = slots[i] / NUM_ROOMS_PER_FLOOR;

            if (!slotUsed[i] &&
                floor == requests[r].preferredFloor) {

                assignedSlot = i;
                break;
            }
        }

        
        
        if (assignedSlot == -1) {

            
            for (int i = 0; i < availableSlots; i++) {
                if (!slotUsed[i]) {
                    assignedSlot = i;
                    break;
                }
            }

            if (assignedSlot == -1) {
                printf("[ERROR] Tidak ada slot tersisa untuk required item\n");
                continue;
            }
        }

        slotUsed[assignedSlot] = true;

        int floor = slots[assignedSlot] / NUM_ROOMS_PER_FLOOR;
        int room  = slots[assignedSlot] % NUM_ROOMS_PER_FLOOR;

        g_items.push_back(
            spawnItemInRoom(
                requests[r].type,
                floor,
                room
            )
        );
    }

    
    int requiredSpawned = 0;
    for (size_t i = 0; i < g_items.size(); i++) {
        if (ITEM_CATALOG[g_items[i].type].isRequired)
            requiredSpawned++;
    }

    printf("\n[Item] Required spawned = %d/5\n", requiredSpawned);

    ItemType bonusItems[] = {
        ITEM_MEDKIT,
        ITEM_MEDKIT,
        ITEM_BATTERY,
        ITEM_BATTERY
    };

    for (int b = 0; b < 4; b++) {

        int assignedSlot = -1;

        for (int i = 0; i < availableSlots; i++) {

            if (!slotUsed[i]) {
                assignedSlot = i;
                break;
            }
        }

        if (assignedSlot == -1)
            break;

        slotUsed[assignedSlot] = true;

        int floor =
            slots[assignedSlot] / NUM_ROOMS_PER_FLOOR;

        int room =
            slots[assignedSlot] % NUM_ROOMS_PER_FLOOR;

        if (lockedRooms[floor][room]) {
            printf("[ERROR] Attempted bonus spawn in LOCKED room floor=%d room=%d\n",
                   floor,
                   room);
        } else {
            g_items.push_back(
                spawnItemInRoom(
                    bonusItems[b],
                    floor,
                    room
                )
            );
        }
    }

    printf("[Item] Spawn selesai. Total item = %d", (int)g_items.size());
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

        float bobOffset = sinf(item.bobTimer * HOVER_SPEED) * HOVER_AMPLITUDE;

        
        glPushMatrix();
            glTranslatef(item.x, item.y + bobOffset, item.z);
            drawItemAura(item.type, item.bobTimer);
        glPopMatrix();

        
        glPushMatrix();
            glTranslatef(item.x, item.y + bobOffset, item.z);
            glRotatef(item.rotAngle, 0.0f, 1.0f, 0.0f);
            glScalef(1.5f, 1.5f, 1.5f);
            setItemColor(item.type);
            drawItemShape(item.type);
        glPopMatrix();
    }
}





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
