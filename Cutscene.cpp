#include "Cutscene.h"
#include "Sound.h"

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

CutsceneManager cutsceneManager;

CutsceneManager::CutsceneManager() 
    : currentDialogIndex(0), elapsedTime(0.0f), isActive(false), hasPlayedBell(false), waitingForInput(false) {}

CutsceneManager::~CutsceneManager() {}

void CutsceneManager::addDialogLine(const string& text, float duration, bool playBell) {
    dialogLines.push_back({text, duration, playBell});
}

void CutsceneManager::startCutscene() {
    if (dialogLines.empty()) return;
    
    isActive = true;
    currentDialogIndex = 0;
    elapsedTime = 0.0f;
    hasPlayedBell = false;
    waitingForInput = true;  // Wait for ENTER on first dialog
    
    if (dialogLines[0].playBellSound) {
        soundManager.playSound(SOUND_BELL);
        hasPlayedBell = true;
    }
}

void CutsceneManager::stopCutscene() {
    isActive = false;
    currentDialogIndex = 0;
    elapsedTime = 0.0f;
    hasPlayedBell = false;
    waitingForInput = false;
    dialogLines.clear();
    soundManager.stopSound(SOUND_BELL);
    soundManager.playSound(SOUND_BACKGROUND);
}

void CutsceneManager::advanceDialog() {
    if (!isActive || dialogLines.empty()) return;
    
    currentDialogIndex++;
    elapsedTime = 0.0f;
    hasPlayedBell = false;
    
    // Check if cutscene finished
    if (currentDialogIndex >= (int)dialogLines.size()) {
        stopCutscene();
        return;
    }
    
    // Play bell sound for next dialog if needed
    if (dialogLines[currentDialogIndex].playBellSound) {
        soundManager.playSound(SOUND_BELL);
        hasPlayedBell = true;
    }
}

void CutsceneManager::update(float deltaTime) {
    if (!isActive || dialogLines.empty()) return;
    
    // No auto-advance - waiting for input
}

void CutsceneManager::render() {
    if (!isActive || currentDialogIndex >= (int)dialogLines.size()) return;
    
    const string& dialogText = dialogLines[currentDialogIndex].text;
    
    // Setup 2D projection for HUD text
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1280, 720, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw semi-transparent background for dialog box
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(100, 550);
        glVertex2f(1180, 550);
        glVertex2f(1180, 680);
        glVertex2f(100, 680);
    glEnd();
    
    // Draw dialog border
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(100, 550);
        glVertex2f(1180, 550);
        glVertex2f(1180, 680);
        glVertex2f(100, 680);
    glEnd();
    
    // Draw "Umar:" name
    glColor3f(1.0f, 0.8f, 0.0f);  // Golden color for name
    glRasterPos2f(120, 620);
    const char* name = "Umar: ";
    for (size_t i = 0; i < strlen(name); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, name[i]);
    }
    
    // Render dialog text
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(250, 620);
    
    for (size_t i = 0; i < dialogText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, dialogText[i]);
    }
    
    // Draw "Press ENTER" hint
    glColor3f(0.7f, 0.7f, 0.7f);
    glRasterPos2f(120, 570);
    const char* hint = "[Press ENTER to continue]";
    for (size_t i = 0; i < strlen(hint); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, hint[i]);
    }
    
    // Restore matrices
    glDisable(GL_BLEND);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

bool CutsceneManager::isRunning() const {
    return isActive;
}

bool CutsceneManager::isWaitingForInput() const {
    return isActive && waitingForInput;
}

const char* CutsceneManager::getCurrentDialogText() const {
    if (!isActive || currentDialogIndex >= (int)dialogLines.size()) {
        return "";
    }
    return dialogLines[currentDialogIndex].text.c_str();
}

float CutsceneManager::getDialogProgress() const {
    if (dialogLines.empty()) return 0.0f;
    return elapsedTime / dialogLines[currentDialogIndex].duration;
}
