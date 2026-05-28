#ifndef DRAWING_H_INCLUDED 
#define DRAWING_H_INCLUDED 

// ==========================================
// RENDERING HELPERS
// ==========================================

// drawBlock 
void drawBlock(float w, float h, float d);

// Wireframe outline 
void drawBlockOutline(float w, float h, float d, float alpha = 1.0f);

// Gambar ground plane berkotak
void drawGround();

// ==========================================
// POHON
// ==========================================

// Gambar pohon (1 pohon)
void drawTree(float x, float z, float trunkH = 2.5f, float crownR = 1.4f);

// Deretan pohon
void drawTrees();

// ==========================================
// MEJA
// ==========================================

// Gambar 1 meja (tabletop + 4 kaki)
void drawDesk(float x, float y, float z);

// Gambar deretan meja dalam 1 ruangan (dipanggil dari Building.cpp)
// roomW = lebar ruangan, roomD = kedalaman ruangan
void drawDeskSet(float roomW, float roomD);
#endif
