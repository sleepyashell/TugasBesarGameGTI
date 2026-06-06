#ifndef JUMPSCARE_H
#define JUMPSCARE_H

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

// Forward declaration for game restart
extern void restartGame();

#include "Sound.h"

// States for jumpscare/gameover sequence
enum JumpscareState {
    JUMPSCARE_NONE,       // Normal gameplay
    JUMPSCARE_PLAYING,    // Showing jumpscare image + sound
    JUMPSCARE_GAMEOVER    // Showing game over menu
};

class JumpscareManager {
private:
    JumpscareState state;
    float timer;              // Timer for jumpscare duration
    float jumpscareDuration;  // How long jumpscare shows (seconds)
    int selectedOption;       // 0 = Restart, 1 = Exit

    GLuint jumpscareTex;      // Texture for jumpscare image
    GLuint gameoverTex;       // Texture for game over (default)
    GLuint gameoverExitTex;   // Texture for game over (exit highlighted)

    bool texturesLoaded;

public:
    JumpscareManager();
    ~JumpscareManager();

    void loadTextures();
    void cleanupTextures();

    void startJumpscare();
    void update(float dt);
    void render();

    void handleInput(int key);

    bool isActive() const;
    bool isGameOver() const;

    void reset();
};

extern JumpscareManager jumpscareManager;

#endif
