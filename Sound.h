#ifndef SOUND_H
#define SOUND_H

#include <iostream>
#include <string>
#include <map>
using namespace std;

enum SoundType {
    SOUND_BELL,
    SOUND_BACKGROUND,
    SOUND_CHASE,
    SOUND_JUMPSCARE
};

class SoundManager {
private:
    bool soundsInitialized;
     map<SoundType, int> soundPIDs;
    
    map<SoundType, bool> soundPlaying;
    
public:
    SoundManager();
    ~SoundManager();
    
    void initialize();
    void playSound(SoundType type);
    void stopSound(SoundType type);
    void stopAllSounds();
    void setVolume(float volume);
    
    bool isPlaying(SoundType type);
    void restartBackgroundIfNeeded();
};

extern SoundManager soundManager;

#endif
