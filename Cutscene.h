#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <vector>
#include <string>
using namespace std;

struct DialogLine {
    string text;
    float duration;
    bool playBellSound;
};

class CutsceneManager {
private:
    vector<DialogLine> dialogLines;
    int currentDialogIndex;
    float elapsedTime;
    bool isActive;
    bool hasPlayedBell;
    bool waitingForInput;  
    
public:
    CutsceneManager();
    ~CutsceneManager();
    
    void addDialogLine(const string& text, float duration, bool playBell = false);
    void startCutscene();
    void stopCutscene();
    void update(float deltaTime);
    void render();
    void advanceDialog();  
    bool isRunning() const;
    bool isWaitingForInput() const;
    const char* getCurrentDialogText() const;
    float getDialogProgress() const;
};

extern CutsceneManager cutsceneManager;

#endif
