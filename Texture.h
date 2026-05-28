#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>

// ==========================================
// TEXTURE MANAGER
// ==========================================

// Load file BMP dan return texture ID OpenGL
// Mengembalikan 0 jika gagal
GLuint loadBMP(const char* filename);

// Hapus semua texture dari memory GPU
void cleanupTextures();

// ==========================================
// POSTER
// ==========================================

// Gambar 1 poster persegi panjang dengan texture
// x,y,z = posisi pojok kiri-bawah poster
// w,h   = lebar dan tinggi poster
// axis  = 'Z' (tembok depan, menghadap +Z), 
//         'z' (tembok belakang, menghadap -Z),
//         'X' (tembok kiri/kanan, menghadap +/-X)
void drawPoster(float x, float y, float z,
                float w, float h,
                char axis, GLuint texID);

// Inisialisasi semua texture poster (dipanggil sekali di init())
void initPosters();

// Gambar poster untuk 1 ruangan tertentu
// f = nomor lantai, roomIndex = indeks ruangan (0..3 untuk 4 poster per lantai)
void drawRoomPosters(int f, int roomIndex);

// Gambar semua poster untuk 1 lantai (4 poster)
void drawFloorPosters(int f);

// Gambar semua poster untuk seluruh gedung
void drawAllPosters();

#endif
