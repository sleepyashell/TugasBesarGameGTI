#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstdio>
#include <cstring>
#include <vector>
#include <fstream>
#include <assert.h>
#include "Texture.h"
#include "World.h"  


using namespace std;

static GLuint g_textures[NUM_TEXTURES] = {0};
static bool g_textureBound = false;

// Kelas Image untuk menyimpan data pixel
class Image {
public:
    char* pixels;
    int width;
    int height;

    Image(char* ps, int w, int h) : pixels(ps), width(w), height(h) {}
    ~Image() { delete[] pixels; }
};

namespace {
    // Konversi 4 buah karakter ke integer, menggunakan bentuk little-endian
    int toInt(const char* bytes) {
        return (int)(((unsigned char)bytes[3] << 24) |
                     ((unsigned char)bytes[2] << 16) |
                     ((unsigned char)bytes[1] << 8) |
                     (unsigned char)bytes[0]);
    }

    // Konversi 2 buah karakter ke integer, menggunakan bentuk little-endian
    short toShort(const char* bytes) {
        return (short)(((unsigned char)bytes[1] << 8) |
                       (unsigned char)bytes[0]);
    }

    // Membaca 4 byte selanjutnya sebagai integer, menggunakan bentuk little-endian
    int readInt(ifstream &input) {
        char buffer[4];
        input.read(buffer, 4);
        return toInt(buffer);
    }

    short readShort(ifstream &input) {
        char buffer[2];
        input.read(buffer, 2);
        return toShort(buffer);
    }

    template<class T>
    class auto_array {
    private:
        T* array;
        mutable bool isReleased;
    public:
        explicit auto_array(T* array_ = NULL) :
            array(array_), isReleased(false) {}

        auto_array(const auto_array<T> &aarray) {
            array = aarray.array;
            isReleased = aarray.isReleased;
            aarray.isReleased = true;
        }

        ~auto_array() {
            if (!isReleased && array != NULL) {
                delete[] array;
            }
        }

        T* get() const { return array; }
        T &operator*() const { return *array; }

        void operator=(const auto_array<T> &aarray) {
            if (!isReleased && array != NULL) {
                delete[] array;
            }
            array = aarray.array;
            isReleased = aarray.isReleased;
            aarray.isReleased = true;
        }

        T* operator->() const { return array; }

        T* release() {
            isReleased = true;
            return array;
        }

        void reset(T* array_ = NULL) {
            if (!isReleased && array != NULL) {
                delete[] array;
            }
            array = array_;
        }

        T* operator+(int i) { return array + i; }
        T &operator[](int i) { return array[i]; }
    };
}


// LOAD BMP - Menggunakan pendekatan imageloader

Image* loadBMPImage(const char* filename) {
    ifstream input;
    input.open(filename, ifstream::binary);
    assert(!input.fail() || !"File tidak ditemukan!!!");

    char buffer[2];
    input.read(buffer, 2);
    assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Bukan file bitmap!!!");

    input.ignore(8);
    int dataOffset = readInt(input);

    int headerSize = readInt(input);
    int width;
    int height;

    switch(headerSize) {
        case 40:
            width = readInt(input);
            height = readInt(input);
            input.ignore(2);
            assert(readShort(input) == 24 || !"Gambar tidak 24 bits per pixel!");
            assert(readShort(input) == 0 || !"Gambar dikompres!");
            break;
        case 12:
            width = readShort(input);
            height = readShort(input);
            input.ignore(2);
            assert(readShort(input) == 24 || !"Gambar tidak 24 bits per pixel!");
            break;
        case 64:
            assert(!"Tidak dapat mengambil OS/2 V2 bitmaps");
            break;
        case 108:
            assert(!"Tidak dapat mengambil Windows V4 bitmaps");
            break;
        case 124:
            assert(!"Tidak dapat mengambil Windows V5 bitmaps");
            break;
        default:
            assert(!"Format bitmap ini tidak diketahui!");
    }

    // Membaca data
    int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
    int size = bytesPerRow * height;
    auto_array<char> pixels(new char[size]);
    input.seekg(dataOffset, ios_base::beg);
    input.read(pixels.get(), size);

    // Mengambil data yang mempunyai format benar (BGR -> RGB)
    auto_array<char> pixels2(new char[width * height * 3]);
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            for(int c = 0; c < 3; c++) {
                pixels2[3 * (width * y + x) + c] =
                    pixels[bytesPerRow * y + 3 * x + (2 - c)];
            }
        }
    }

    input.close();
    return new Image(pixels2.release(), width, height);
}

// TEXTURE LOADER - Wrapper untuk OpenGL

GLuint loadBMP(const char* filename) {
    Image* img = loadBMPImage(filename);
    if (!img) {
        printf("[Texture] Gagal membuka: %s\n", filename);
        return 0;
    }

    // Upload ke GPU
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 img->width, img->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

    printf("[Texture] Loaded: %s (id=%u, %dx%d)\n", filename, texID, img->width, img->height);

    // Hapus image setelah upload ke GPU
    delete img;

    return texID;
}


// 4 poster yang diulang untuk tiap lantai
static GLuint g_posterTex[4] = { 0, 0, 0, 0 };

static const char* POSTER_FILES[] = {
    "poster1.bmp",    // poster ruangan 1
    "poster2.bmp",    // poster ruangan 2
    "poster3.bmp",    // poster ruangan 3
    "poster4.bmp"     // poster ruangan 4
};
static const int NUM_POSTERS = 4;

void initPosters() {
    for (int i = 0; i < NUM_POSTERS; i++) {
        g_posterTex[i] = loadBMP(POSTER_FILES[i]);
    }
}


// POSTER RENDERER

void drawPoster(float x, float y, float z,
                float w, float h,
                char axis, GLuint texID, float alpha) { 
    if (texID == 0) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Jika sedang tembus pandang, matikan lighting sementara seperti pada tembok
    if (alpha < 1.0f) {
        glDisable(GL_LIGHTING);
    }

    GLfloat white[] = { 1.0f, 1.0f, 1.0f, alpha }; 
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  white);
    GLfloat noSpec[] = { 0.0f, 0.0f, 0.0f, alpha };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, noSpec);

    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    glBegin(GL_QUADS);
    if (axis == 'Z') {
        glTexCoord2f(0, 0); glVertex3f(x,     y,     z);
        glTexCoord2f(1, 0); glVertex3f(x + w, y,     z);
        glTexCoord2f(1, 1); glVertex3f(x + w, y + h, z);
        glTexCoord2f(0, 1); glVertex3f(x,     y + h, z);
    } else if (axis == 'z') {
        glTexCoord2f(0, 0); glVertex3f(x,     y,     z);
        glTexCoord2f(0, 1); glVertex3f(x,     y + h, z);
        glTexCoord2f(1, 1); glVertex3f(x + w, y + h, z);
        glTexCoord2f(1, 1); glVertex3f(x + w, y,     z);
    } else {
        glTexCoord2f(0, 0); glVertex3f(x, y,     z);
        glTexCoord2f(1, 0); glVertex3f(x, y,     z - w);
        glTexCoord2f(1, 1); glVertex3f(x, y + h, z - w);
        glTexCoord2f(0, 1); glVertex3f(x, y + h, z);
    }
    glEnd();

    if (alpha < 1.0f) {
        glEnable(GL_LIGHTING);
    }

    glDisable(GL_TEXTURE_2D);
}

// LAYOUT POSTER PER RUANGAN (4 POSTER PER LANTAI)

// Poster dipasang di dinding depan (luar) setiap ruangan
// Room 0: x=0..8,  Room 1: x=8..16,  Room 2: x=16..24
// Room 3: x=40..48 (skip area tangga x=32..40)

void drawRoomPosters(int f, int roomIndex) {
    float fy = f * FLOOR_HEIGHT;
    float py = fy + 1.0f;           

    float pw = 1.5f;                
    float ph = 2.0f;                
    float pz = 0.05f;               

    GLuint texID = g_posterTex[roomIndex % NUM_POSTERS];
    if (texID == 0) return;  

    // Hitung posisi batas X ruangan untuk deteksi player
    float startX = roomIndex * 8.0f;
    
    // PERBAIKAN: Diubah dari 37.0f menjadi 36.0f agar sinkron dengan startX di Building.cpp
    if (roomIndex >= 3) startX = 36.0f; 
    
    float roomWidth = 8.0f;

    // === HITUNG ALPHA SAMA PERSIS DENGAN TEMBOK DEPAN ===
    bool insideZ   = (playerZ < 0.0f);
    bool insideX   = (playerX >= startX && playerX <= startX + roomWidth);
    bool sameFloor = (playerY >= fy && playerY < fy + FLOOR_HEIGHT);
    float alpha     = (insideZ && insideX && sameFloor) ? 0.05f : 1.0f;

    // Tentukan koordinat tengah X untuk meletakkan poster
    float roomX = startX + 4.0f;
    float px = roomX - pw / 2.0f;  

    // Oper nilai alpha ke fungsi drawPoster
    drawPoster(px, py, pz, pw, ph, 'Z', texID, alpha);
}

void drawFloorPosters(int f) {
    // Gambar 4 poster untuk lantai ini
    for (int room = 0; room < 4; room++) {
        drawRoomPosters(f, room);
    }
}

void drawAllPosters() {
    for (int f = 0; f < NUM_FLOORS; f++) {
        drawFloorPosters(f);
    }
}

void initTextures() {
    
    // Load texture dinding
    g_textures[TEX_WALL] = loadBMP("wall.bmp");  // test dulu
    
    // Load texture ubin lantai
    g_textures[TEX_FLOOR_TILE] = loadBMP("floor_title.bmp");
    
    // Load texture poster
    g_textures[TEX_POSTER_1] = loadBMP("poster1.bmp");
    g_textures[TEX_POSTER_2] = loadBMP("poster2.bmp");
    g_textures[TEX_POSTER_3] = loadBMP("poster3.bmp");
    g_textures[TEX_POSTER_4] = loadBMP("poster4.bmp");
}

void bindTexture(TextureID texID) {
    if (texID >= 0 && texID < NUM_TEXTURES && g_textures[texID] != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_textures[texID]);
        
        // Pastikan texture parameter benar
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        g_textureBound = true;
        
        // BARIS PRINTF LOG SUDAH DIHAPUS BIAR RE-RENDER GRAFIS ENTEK GAK PUTIH POLOS
    } else {
        printf("[Texture] ERROR: Cannot bind texture %d (ID=%u)\n", texID, 
               (texID >= 0 && texID < NUM_TEXTURES) ? g_textures[texID] : 999);
    }
}

void unbindTexture() {
    if (g_textureBound) {
        glDisable(GL_TEXTURE_2D);
        g_textureBound = false;
    }
}

void cleanupTextures() {
    glDeleteTextures(NUM_TEXTURES, g_textures);
    for (int i = 0; i < NUM_TEXTURES; i++) {
        g_textures[i] = 0;
    }
}
