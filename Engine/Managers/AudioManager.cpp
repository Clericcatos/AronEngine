#include "framework.h"
#include "AudioManager.h"
#include "../Resources/AudioClip.h"
#include "../../ThirdParty/FMOD/inc/fmod.hpp"
#include "../../ThirdParty/FMOD/inc/fmod_errors.h"
#include <algorithm>

// #pragma comment(lib, "fmod_vc.lib") // Using dummy implementation for now

namespace AronEngine
{
    AudioManager::AudioManager()
        : fmodSystem(nullptr)
        , masterGroup(nullptr)
        , initialized(false)
    {
    }

    AudioManager::~AudioManager()
    {
        Shutdown();
    }

    bool AudioManager::Initialize(void* windowHandle)
    {
        if (initialized) return true;

        if (!CreateFMODSystem())
        {
            DEBUG_LOG("Failed to create FMOD System");
            return false;
        }

        if (!InitializeFMODSystem())
        {
            DEBUG_LOG("Failed to initialize FMOD System");
            return false;
        }

        // Get master channel group
        FMOD_RESULT result = fmodSystem->getMasterChannelGroup(&masterGroup);
        if (result != FMOD_OK)
        {
            DEBUG_LOG("Failed to get master channel group: " + std::string(FMOD_ErrorString(result)));
            return false;
        }

        initialized = true;
        DEBUG_LOG("AudioManager initialized successfully with FMOD");
        return true;
    }

    void AudioManager::Shutdown()
    {
        if (!initialized) return;

        UnloadAllAudioClips();

        if (fmodSystem)
        {
            fmodSystem->release();
            fmodSystem = nullptr;
        }

        masterGroup = nullptr;
        initialized = false;
        DEBUG_LOG("AudioManager shutdown");
    }

    void AudioManager::Update()
    {
        if (fmodSystem)
        {
            fmodSystem->update();
        }
    }

    std::shared_ptr<AudioClip> AudioManager::LoadAudioClip(const std::string& filePath, bool is3D, bool isLooping, bool isStream)
    {
        if (!initialized)
        {
            DEBUG_LOG("AudioManager not initialized, cannot load audio clip: " + filePath);
            return nullptr;
        }

        // Create a unique key for this audio clip configuration
        std::string key = filePath + "_" + (is3D ? "3D" : "2D") + "_" + (isLooping ? "Loop" : "Once") + "_" + (isStream ? "Stream" : "Sample");
        
        // Check if already loaded with these settings
        auto it = audioClips.find(key);
        if (it != audioClips.end())
        {
            DEBUG_LOG("Audio clip already loaded: " + key);
            return it->second;
        }

        // Create and load new audio clip
        auto audioClip = std::make_shared<AudioClip>();
        if (audioClip->LoadFromFile(filePath, is3D, isLooping, isStream))
        {
            audioClips[key] = audioClip;
            DEBUG_LOG("Audio clip loaded successfully: " + filePath);
            return audioClip;
        }
        else
        {
            DEBUG_LOG("Failed to load audio clip: " + filePath);
            return nullptr;
        }
    }

    void AudioManager::UnloadAudioClip(const std::string& filePath)
    {
        // Remove all variations of this audio clip
        auto it = audioClips.begin();
        while (it != audioClips.end())
        {
            if (it->first.find(filePath) == 0) // Starts with the file path
            {
                it->second->Unload();
                it = audioClips.erase(it);
                DEBUG_LOG("Audio clip unloaded: " + filePath);
            }
            else
            {
                ++it;
            }
        }
    }

    void AudioManager::UnloadAllAudioClips()
    {
        for (auto& pair : audioClips)
        {
            pair.second->Unload();
        }
        audioClips.clear();
        DEBUG_LOG("All audio clips unloaded");
    }

    void AudioManager::SetMasterVolume(float volume)
    {
        if (!masterGroup) return;

        // Clamp volume between 0.0f and 1.0f
        volume = std::max(0.0f, std::min(1.0f, volume));
        
        FMOD_RESULT result = masterGroup->setVolume(volume);
        
        if (result == FMOD_OK)
        {
            DEBUG_LOG("Master volume set to: " + std::to_string(volume));
        }
        else
        {
            DEBUG_LOG("Failed to set master volume: " + std::string(FMOD_ErrorString(result)));
        }
    }

    float AudioManager::GetMasterVolume() const
    {
        if (!masterGroup) return 0.0f;

        float volume = 0.0f;
        FMOD_RESULT result = masterGroup->getVolume(&volume);
        
        if (result == FMOD_OK)
        {
            return volume;
        }
        
        return 0.0f;
    }

    void AudioManager::Set3DListenerPosition(float x, float y, float z)
    {
        if (!fmodSystem) return;

        FMOD_VECTOR pos = { x, y, z };
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
        FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
        FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };

        fmodSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
    }

    void AudioManager::Set3DListenerVelocity(float vx, float vy, float vz)
    {
        if (!fmodSystem) return;

        FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f };
        FMOD_VECTOR vel = { vx, vy, vz };
        FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
        FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };

        fmodSystem->get3DListenerAttributes(0, &pos, nullptr, nullptr, nullptr);
        fmodSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
    }

    void AudioManager::Set3DListenerOrientation(float forwardX, float forwardY, float forwardZ,
                                               float upX, float upY, float upZ)
    {
        if (!fmodSystem) return;

        FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f };
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
        FMOD_VECTOR forward = { forwardX, forwardY, forwardZ };
        FMOD_VECTOR up = { upX, upY, upZ };

        fmodSystem->get3DListenerAttributes(0, &pos, &vel, nullptr, nullptr);
        fmodSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
    }

    bool AudioManager::CreateFMODSystem()
    {
        FMOD_RESULT result = FMOD::System_Create(&fmodSystem, FMOD_VERSION);
        if (result != FMOD_OK)
        {
            DEBUG_LOG("FMOD error! " + std::string(FMOD_ErrorString(result)));
            return false;
        }

        // Check FMOD version
        unsigned int version;
        result = fmodSystem->getVersion(&version);
        if (result != FMOD_OK)
        {
            DEBUG_LOG("FMOD error! " + std::string(FMOD_ErrorString(result)));
            return false;
        }

        if (version < FMOD_VERSION)
        {
            DEBUG_LOG("FMOD lib version doesn't match header version!");
            return false;
        }

        return true;
    }

    bool AudioManager::InitializeFMODSystem()
    {
        // Initialize FMOD with 512 virtual channels
        FMOD_RESULT result = fmodSystem->init(512, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            DEBUG_LOG("FMOD error! " + std::string(FMOD_ErrorString(result)));
            return false;
        }

        // Set 3D settings
        result = fmodSystem->set3DSettings(1.0f, 1.0f, 1.0f);
        if (result != FMOD_OK)
        {
            DEBUG_LOG("Failed to set 3D settings: " + std::string(FMOD_ErrorString(result)));
        }

        return true;
    }
}