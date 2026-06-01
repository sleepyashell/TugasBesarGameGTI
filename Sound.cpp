#include "Sound.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
    #include <windows.h>
    #include <mmsystem.h>
    #pragma comment(lib, "winmm.lib")
#endif

SoundManager soundManager;

SoundManager::SoundManager() : soundsInitialized(false) {
    // Inisialisasi map manual untuk C++98
    soundPIDs[SOUND_BELL] = -1;
    soundPIDs[SOUND_BACKGROUND] = -1;
    soundPIDs[SOUND_CHASE] = -1;

    soundPlaying[SOUND_BELL] = false;
    soundPlaying[SOUND_BACKGROUND] = false;
    soundPlaying[SOUND_CHASE] = false;
}

SoundManager::~SoundManager() {
    stopAllSounds();
}

void SoundManager::initialize() {
    soundsInitialized = true;
}

bool SoundManager::isPlaying(SoundType type) {
    std::map<SoundType, bool>::iterator it = soundPlaying.find(type);
    if (it != soundPlaying.end()) {
        return it->second;
    }
    return false;
}

void SoundManager::playSound(SoundType type) {
    if (!soundsInitialized) return;

    // Jika suara ini sudah diputar, jangan diputar ulang
    if (isPlaying(type)) return;

    #ifdef _WIN32
        switch(type) {
            case SOUND_BELL:
                PlaySoundA("Sound\\bellsound.wav", NULL, SND_FILENAME | SND_ASYNC);
                break;
            case SOUND_BACKGROUND:
                PlaySoundA("Sound\\backsound.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
                break;
            case SOUND_CHASE:
                PlaySoundA("Sound\\chasesound.wav", NULL, SND_FILENAME | SND_ASYNC);
                break;
        }
        soundPlaying[type] = true;
    #else
        // Mac/Linux - gunakan afplay dengan PID tracking
        const char* filename = NULL;
        bool loop = false;

        switch(type) {
            case SOUND_BELL:
                filename = "Sound/bellsound.wav";
                loop = false;
                break;
            case SOUND_BACKGROUND:
                filename = "Sound/backsound.wav";
                loop = true;
                break;
            case SOUND_CHASE:
                filename = "Sound/chasesound.wav";
                loop = false;
                break;
        }

        if (filename) {
            char cmd[256];
            if (loop) {
                // PERBAIKAN: Gunakan afplay -l 0 untuk loop infinite
                // -l 0 = loop forever, 1 proses afplay saja yang handle loop internal
                snprintf(cmd, sizeof(cmd), 
                    "afplay -l 0 \"%s\" > /dev/null 2>&1 & echo $!", 
                    filename);
            } else {
                snprintf(cmd, sizeof(cmd), 
                    "afplay \"%s\" > /dev/null 2>&1 & echo $!", 
                    filename);
            }

            FILE* pipe = popen(cmd, "r");
            if (pipe) {
                char buffer[32];
                if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                    int pid = atoi(buffer);
                    soundPIDs[type] = pid;
                    soundPlaying[type] = true;
                }
                pclose(pipe);
            }
        }
    #endif
}

void SoundManager::stopSound(SoundType type) {
    #ifdef _WIN32
        // Windows: PlaySoundA(NULL, NULL, 0) menghentikan SEMUA suara
        // Ini adalah limitasi Windows API
        PlaySoundA(NULL, NULL, 0);
        // Reset semua status playing
        soundPlaying[SOUND_BELL] = false;
        soundPlaying[SOUND_BACKGROUND] = false;
        soundPlaying[SOUND_CHASE] = false;
    #else
        // Mac/Linux: Hentikan hanya PID yang ditarget
        std::map<SoundType, int>::iterator it = soundPIDs.find(type);
        if (it != soundPIDs.end()) {
            int pid = it->second;
            if (pid > 0) {
                char cmd[64];
                snprintf(cmd, sizeof(cmd), "kill %d > /dev/null 2>&1", pid);
                system(cmd);
                it->second = -1;
            }
        }

        std::map<SoundType, bool>::iterator it2 = soundPlaying.find(type);
        if (it2 != soundPlaying.end()) {
            it2->second = false;
        }
    #endif
}

void SoundManager::stopAllSounds() {
    #ifdef _WIN32
        PlaySoundA(NULL, NULL, 0);
    #else
        // Hentikan semua PID yang tracked
        std::map<SoundType, int>::iterator it;
        for (it = soundPIDs.begin(); it != soundPIDs.end(); ++it) {
            int pid = it->second;
            if (pid > 0) {
                char cmd[64];
                snprintf(cmd, sizeof(cmd), "kill %d > /dev/null 2>&1", pid);
                system(cmd);
            }
        }
        // Fallback: kill semua afplay yang tersisa
        system("pkill -f 'afplay' > /dev/null 2>&1");
    #endif

    // Reset status
    std::map<SoundType, bool>::iterator it2;
    for (it2 = soundPlaying.begin(); it2 != soundPlaying.end(); ++it2) {
        it2->second = false;
    }

    std::map<SoundType, int>::iterator it3;
    for (it3 = soundPIDs.begin(); it3 != soundPIDs.end(); ++it3) {
        it3->second = -1;
    }
}

void SoundManager::restartBackgroundIfNeeded() {
    // Restart backsound jika tidak sedang diputar
    if (!isPlaying(SOUND_BACKGROUND)) {
        playSound(SOUND_BACKGROUND);
    }
}

void SoundManager::setVolume(float volume) {
    // Volume control would require additional implementation
}
