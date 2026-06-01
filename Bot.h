#ifndef BOT_H_INCLUDED
#define BOT_H_INCLUDED

// =========================================================================
// DATA STRUCTURES AI BOT GHOST
// =========================================================================

// Struct Waypoint bawaan kelompokmu
struct Waypoint {
    float x;
    float y;
    float z;
};

// Struct data state komponen EnemyBot
struct EnemyBot {
    float x, y, z;
    float speed;
    float dirX;
    int currentFloor;
    bool movingRight;
    bool isChasing;
    bool isSearching;
    int targetNodeIndex;
};

// Array global untuk menampung 3 hantu (Lantai 1, Lantai 2, Lantai 3)
extern EnemyBot ghostBots[3];

// =========================================================================
// PROTOTIPE FUNGSI MANAJEMEN AI ARRAY
// =========================================================================
void initBot();
void updateBot();
void drawBot();

// Fungsi pendukung pergerakan internal (menerima parameter indeks bot)
bool scanForPlayer(int botIdx);
void executeChaseMode(int botIdx, int playerFloor);
void executeCorridorPatrol(int botIdx);

#endif
