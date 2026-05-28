#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include <vector>
#include <string>
using namespace std;
// ==========================================
// MODULE ITEM
// ==========================================

// Jenis-jenis item yang bisa diambil
enum ItemType {
    ITEM_KEYCARD,       // Kartu akses untuk keluar gedung
    ITEM_FLASHLIGHT,    // Senter (menambah radius cahaya)
    ITEM_DOCUMENT,      // Dokumen penting (bukti/clue)
    ITEM_MEDKIT,        // Kotak P3K (restore health di masa depan)
    ITEM_BATTERY,       // Baterai (untuk senter)
    NUM_ITEM_TYPES
};

// Data satu item di dunia
struct WorldItem {
    ItemType    type;
    float       x, y, z;       // posisi dunia
    bool        collected;      // sudah diambil?
    float       bobTimer;       // timer untuk animasi mengambang (bobbing)
    float       rotAngle;       // sudut rotasi item
    int         floor;          // di lantai berapa item ini berada
};

// Info item untuk HUD / inventory
struct ItemInfo {
    ItemType    type;
    string name;
    string description;
    bool        isRequired;     // wajib dikumpulkan untuk menang?
};

// Inisialisasi sistem item, spawn item secara random di seluruh gedung
// Panggil sekali di init() setelah buildPhysicalWorld()
void initItems();

// Update animasi item setiap frame
void updateItems(float dt);


void checkItemPickup();

// Render semua item yang belum diambil di dunia
void drawItems();

// Kembalikan jumlah item required yang sudah dikumpulkan
int getCollectedRequiredCount();

// Kembalikan total item required yang harus dikumpulkan
int getTotalRequiredCount();

// Apakah semua item required sudah terkumpul? (syarat menang)
bool allRequiredItemsCollected();

// Reset semua item (untuk restart game)
void resetItems();

#endif
