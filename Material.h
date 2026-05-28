#ifndef MATERIAL_H_INCLUDED 
#define MATERIAL_H_INCLUDED 

// Set material property 
void setMaterial(float r, float g, float b, float a,
                 float specR = 0.2f, float specG = 0.2f, float specB = 0.2f,
                 float shininess = 16.0f);

// --- HORROR PALETTE ---
// Concrete / dinding
void matConcrete();
// Lantai
void matFloor();
// Plafon
void matCeiling();
// Tiang / pilar
void matPillar();
// Tangga
void matStair();
// Pintu kayu
void matDoor();
// Handle pintu
void matHandle();
// Batang pohon
void matTrunk();
// Daun pohon
void matLeaf();
// Pemain
void matPlayer();
// Tanah
void matGround();
// Meja (kayu gelap)
void matDesk();
// Kaki meja (metal)
void matDeskLeg();
#endif
