#include "framework.h"
#include "AudioClip.h"
#include "../Managers/AudioManager.h"
#include "../../ThirdParty/FMOD/inc/fmod.hpp"
#include "../../ThirdParty/FMOD/inc/fmod_errors.h"
#include <algorithm>
#include <filesystem>

namespace AronEngine
{
    AudioClip::AudioClip()
        : filePath("")
        , format(AudioFormat::UNKNOWN)
        , fmodSound(nullptr)
        , length(0.0f)
        , frequency(0)
        , channels(0)
        , isLoaded(false)
        , is3D(false)
        , isLooping(false)
        , isStream(false)
    {
    }

    AudioClip::AudioClip(const std::string& path)
        : AudioClip()
    {
        LoadFromFile(path);
    }

    AudioClip::~AudioClip()
    {
        Unload();
    }

    bool AudioClip::LoadFromFile(const std::string& path, bool is3D, bool isLooping, bool isStream)
    {
        if (isLoaded)
        {
            Unload();
        }

        // Get FMOD system
        FMOD::System* fmodSystem = AudioManager::GetInstance().GetFMODSystem();
        if (!fmodSystem)
        {
            DEBUG_LOG("Failed to get FMOD system");
            return false;
        }

        // Check if file exists
        if (!std::filesystem::exists(path))
        {
            DEBUG_LOG("Audio file not found: " + path);
            return false;
        }

        // Detect format
        format = DetectFormat(path);

        // Set up FMOD creation flags
        int mode = FMOD_DEFAULT;
        
        if (is3D)
            mode |= FMOD_3D;
        else
            mode |= FMOD_2D;

        if (isLooping)
            mode |= FMOD_LOOP_NORMAL;
        else
            mode |= FMOD_LOOP_OFF;

        if (isStream)
            mode |= FMOD_CREATESTREAM;
        else
            mode |= FMOD_CREATESAMPLE;

        // Create sound
        FMOD_RESULT result = fmodSystem->createSound(path.c_str(), mode, nullptr, &fmodSound);
        if (result != FMOD_OK)
        {
            DEBUG_LOG("Failed to create sound: " + std::string(FMOD_ErrorString(result)));
            return false;
        }

        // Store properties
        filePath = path;
        this->is3D = is3D;
        this->isLooping = isLooping;
        this->isStream = isStream;

        // Update audio properties
        UpdateAudioProperties();

        isLoaded = true;
        DEBUG_LOG("AudioClip loaded: " + path);
        return true;
    }

    void AudioClip::Unload()
    {
        if (!isLoaded) return;

        if (fmodSound)
        {
            fmodSound->release();
            fmodSound = nullptr;
        }

        filePath = "";
        format = AudioFormat::UNKNOWN;
        length = 0.0f;
        frequency = 0;
        channels = 0;
        isLoaded = false;
        is3D = false;
        isLooping = false;
        isStream = false;

        DEBUG_LOG("AudioClip unloaded");
    }

    AudioFormat AudioClip::DetectFormat(const std::string& path)
    {
        std::string extension = std::filesystem::path(path).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        if (extension == ".wav")
            return AudioFormat::WAV;
        else if (extension == ".mp3")
            return AudioFormat::MP3;
        else if (extension == ".ogg")
            return AudioFormat::OGG;
        else if (extension == ".flac")
            return AudioFormat::FLAC;
        else
            return AudioFormat::UNKNOWN;
    }

    void AudioClip::UpdateAudioProperties()
    {
        if (!fmodSound) return;

        // Get length in milliseconds
        unsigned int lengthMs;
        FMOD_RESULT result = fmodSound->getLength(&lengthMs, FMOD_TIMEUNIT_MS);
        if (result == FMOD_OK)
        {
            length = lengthMs / 1000.0f; // Convert to seconds
        }

        // Get format info
        FMOD_SOUND_TYPE type;
        FMOD_SOUND_FORMAT format;
        int tempChannels, tempBits;
        
        result = fmodSound->getFormat(&type, &format, &tempChannels, &tempBits);
        if (result == FMOD_OK)
        {
            channels = tempChannels;
        }

        // Get frequency
        float tempFreq;
        result = fmodSound->getDefaults(&tempFreq, nullptr);
        if (result == FMOD_OK)
        {
            frequency = static_cast<int>(tempFreq);
        }
    }
}