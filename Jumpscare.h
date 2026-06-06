#ifndef JUMPSCARE_H
#define JUMPSCARE_H

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif


extern void restartGame();

#include "Sound.h"


enum JumpscareState {
    JUMPSCARE_NONE,       
    JUMPSCARE_PLAYING,    
    JUMPSCARE_GAMEOVER    
};

class JumpscareManager {
private:
    JumpscareState state;
    float timer;              
    float jumpscareDuration;  
    int selectedOption;       

    GLuint jumpscareTex;      
    GLuint gameoverTex;       
    GLuint gameoverExitTex;   

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
