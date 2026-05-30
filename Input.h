#ifndef INPUT_H
#define INPUT_H

// Deklarasi fungsi callback GLUT sesuai standar modul
void pressNormalKeys(unsigned char key, int x, int y);
void releaseNormalKeys(unsigned char key, int x, int y);

// Fungsi pembungkus logika yang dipanggil di update loop
void inputMovement();
void inputMenu();
void handleAllInput();

#endif
