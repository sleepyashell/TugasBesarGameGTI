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

#ifdef _WIN32
// Dapatkan path absolut ke file sound berdasarkan lokasi .exe
static std::string getSoundPath(const char* filename) {
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    // Potong nama file .exe, ambil folder-nya saja
    char* lastSlash = strrchr(exePath, '\\');
    if (lastSlash) *(lastSlash + 1) = '\0';
    return std::string(exePath) + "Sound\\" + filename;
}
#endif

SoundManager::SoundManager() : soundsInitialized(false) {
    soundPIDs[SOUND_BELL]       = -1;
    soundPIDs[SOUND_BACKGROUND] = -1;
    soundPIDs[SOUND_CHASE]      = -1;
    soundPIDs[SOUND_JUMPSCARE]  = -1;

    soundPlaying[SOUND_BELL]       = false;
    soundPlaying[SOUND_BACKGROUND] = false;
    soundPlaying[SOUND_CHASE]      = false;
    soundPlaying[SOUND_JUMPSCARE]  = false;
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

    if (isPlaying(type)) return;

    #ifdef _WIN32
        std::string path;
        switch(type) {
            case SOUND_BELL:
                path = getSoundPath("bellsound.wav");
                PlaySoundA(path.c_str(), NULL, SND_FILENAME | SND_ASYNC);
                break;
            case SOUND_BACKGROUND:
                path = getSoundPath("backsound.wav");
                PlaySoundA(path.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
                break;
            case SOUND_CHASE:
                path = getSoundPath("chasesound.wav");
                PlaySoundA(path.c_str(), NULL, SND_FILENAME | SND_ASYNC);
                break;
            case SOUND_JUMPSCARE:
                path = getSoundPath("JumpscareSound.wav");
                PlaySoundA(path.c_str(), NULL, SND_FILENAME | SND_ASYNC);
                break;
        }
        printf("[Sound] Playing: %s\n", path.c_str());
        soundPlaying[type] = true;
    #else
        const char* filename = NULL;
        bool loop = false;

        switch(type) {
            case SOUND_BELL:       filename = "Sound/bellsound.wav";      loop = false; break;
            case SOUND_BACKGROUND: filename = "Sound/backsound.wav";      loop = true;  break;
            case SOUND_CHASE:      filename = "Sound/chasesound.wav";     loop = false; break;
            case SOUND_JUMPSCARE:  filename = "Sound/JumpscareSound.wav"; loop = false; break;
        }

        if (filename) {
            char cmd[256];
            if (loop) {
                snprintf(cmd, sizeof(cmd), "afplay -l 0 \"%s\" > /dev/null 2>&1 & echo $!", filename);
            } else {
                snprintf(cmd, sizeof(cmd), "afplay \"%s\" > /dev/null 2>&1 & echo $!", filename);
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
        PlaySoundA(NULL, NULL, 0);
        soundPlaying[SOUND_BELL]       = false;
        soundPlaying[SOUND_BACKGROUND] = false;
        soundPlaying[SOUND_CHASE]      = false;
        soundPlaying[SOUND_JUMPSCARE]  = false;
    #else
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
        std::map<SoundType, int>::iterator it;
        for (it = soundPIDs.begin(); it != soundPIDs.end(); ++it) {
            int pid = it->second;
            if (pid > 0) {
                char cmd[64];
                snprintf(cmd, sizeof(cmd), "kill %d > /dev/null 2>&1", pid);
                system(cmd);
            }
        }
        system("pkill -f 'afplay' > /dev/null 2>&1");
    #endif

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
    if (!isPlaying(SOUND_BACKGROUND)) {
        playSound(SOUND_BACKGROUND);
    }
}

void SoundManager::setVolume(float volume) {
    // Volume control would require additional implementation
}
