#ifndef LIGHTING_H
#define LIGHTING_H

extern float flickerIntensity;

// Daftarkan prototipe fungsi agar Main.cpp bisa mengaksesnya
void setupLighting();
void updateLightPositions();
void handleAllLighting();

#endif
