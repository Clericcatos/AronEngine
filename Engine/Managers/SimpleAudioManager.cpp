#include "framework.h"
#include "SimpleAudioManager.h"
#include <fstream>

namespace AronEngine
{
    // SimpleAudioManager implementation
    SimpleAudioManager::SimpleAudioManager()
        : initialized(false)
    {
    }

    SimpleAudioManager::~SimpleAudioManager()
    {
        Shutdown();
    }

    bool SimpleAudioManager::Initialize()
    {
        if (initialized) return true;

        DEBUG_LOG("Simple AudioManager initialized (using PlaySound API)");
        initialized = true;
        return true;
    }

    void SimpleAudioManager::Shutdown()
    {
        if (!initialized) return;

        StopAllSounds();
        UnloadAllAudioClips();
        
        initialized = false;
        DEBUG_LOG("Simple AudioManager shutdown");
    }

    std::shared_ptr<SimpleAudioClip> SimpleAudioManager::LoadAudioClip(const std::string& filePath)
    {
        if (!initialized)
        {
            DEBUG_LOG("AudioManager not initialized, cannot load audio clip: " + filePath);
            return nullptr;
        }

        // Check if already loaded
        auto it = audioClips.find(filePath);
        if (it != audioClips.end())
        {
            DEBUG_LOG("Audio clip already loaded: " + filePath);
            return it->second;
        }

        // Create and load new audio clip
        auto audioClip = std::make_shared<SimpleAudioClip>();
        if (audioClip->LoadFromFile(filePath))
        {
            audioClips[filePath] = audioClip;
            DEBUG_LOG("Audio clip loaded successfully: " + filePath);
            return audioClip;
        }
        else
        {
            DEBUG_LOG("Failed to load audio clip: " + filePath);
            return nullptr;
        }
    }

    void SimpleAudioManager::UnloadAudioClip(const std::string& filePath)
    {
        auto it = audioClips.find(filePath);
        if (it != audioClips.end())
        {
            it->second->Unload();
            audioClips.erase(it);
            DEBUG_LOG("Audio clip unloaded: " + filePath);
        }
    }

    void SimpleAudioManager::UnloadAllAudioClips()
    {
        for (auto& pair : audioClips)
        {
            pair.second->Unload();
        }
        audioClips.clear();
        DEBUG_LOG("All audio clips unloaded");
    }

    bool SimpleAudioManager::PlaySound(const std::string& filePath, bool async)
    {
        if (!initialized) return false;

        // Check if file exists
        std::ifstream file(filePath);
        if (!file.good())
        {
            DEBUG_LOG("Audio file not found: " + filePath);
            return false;
        }
        file.close();

        // Convert to wide string
        std::wstring wideFilePath(filePath.begin(), filePath.end());

        DWORD flags = SND_FILENAME;
        if (async) flags |= SND_ASYNC;
        else flags |= SND_SYNC;

        BOOL result = ::PlaySound(wideFilePath.c_str(), nullptr, flags);
        
        if (result)
        {
            DEBUG_LOG("Playing sound: " + filePath);
            return true;
        }
        else
        {
            DEBUG_LOG("Failed to play sound: " + filePath);
            return false;
        }
    }

    bool SimpleAudioManager::PlaySoundLoop(const std::string& filePath)
    {
        if (!initialized) return false;

        std::ifstream file(filePath);
        if (!file.good())
        {
            DEBUG_LOG("Audio file not found: " + filePath);
            return false;
        }
        file.close();

        std::wstring wideFilePath(filePath.begin(), filePath.end());
        BOOL result = ::PlaySound(wideFilePath.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP);
        
        if (result)
        {
            DEBUG_LOG("Playing looped sound: " + filePath);
            return true;
        }
        else
        {
            DEBUG_LOG("Failed to play looped sound: " + filePath);
            return false;
        }
    }

    void SimpleAudioManager::StopAllSounds()
    {
        ::PlaySound(nullptr, nullptr, SND_PURGE);
        DEBUG_LOG("All sounds stopped");
    }

    // SimpleAudioClip implementation
    SimpleAudioClip::SimpleAudioClip()
        : isLoaded(false)
        , length(0.0f)
    {
    }

    SimpleAudioClip::SimpleAudioClip(const std::string& path)
        : SimpleAudioClip()
    {
        LoadFromFile(path);
    }

    SimpleAudioClip::~SimpleAudioClip()
    {
        Unload();
    }

    bool SimpleAudioClip::LoadFromFile(const std::string& path)
    {
        filePath = path;
        
        // Check if file exists
        std::ifstream file(path);
        if (!file.good())
        {
            DEBUG_LOG("Audio file not found: " + path);
            return false;
        }
        file.close();

        // For now, assume it's valid and set a default length
        length = 1.0f; // Default to 1 second
        isLoaded = true;
        
        DEBUG_LOG("SimpleAudioClip loaded: " + path);
        return true;
    }

    void SimpleAudioClip::Unload()
    {
        if (isLoaded)
        {
            Stop();
            isLoaded = false;
            DEBUG_LOG("SimpleAudioClip unloaded: " + filePath);
        }
    }

    bool SimpleAudioClip::Play(bool async)
    {
        if (!isLoaded) return false;
        
        std::wstring wideFilePath(filePath.begin(), filePath.end());
        DWORD flags = SND_FILENAME;
        if (async) flags |= SND_ASYNC;
        else flags |= SND_SYNC;

        BOOL result = ::PlaySound(wideFilePath.c_str(), nullptr, flags);
        
        if (result)
        {
            DEBUG_LOG("SimpleAudioClip playing: " + filePath);
            return true;
        }
        else
        {
            DEBUG_LOG("SimpleAudioClip failed to play: " + filePath);
            return false;
        }
    }

    bool SimpleAudioClip::PlayLoop()
    {
        if (!isLoaded) return false;
        
        std::wstring wideFilePath(filePath.begin(), filePath.end());
        BOOL result = ::PlaySound(wideFilePath.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP);
        
        if (result)
        {
            DEBUG_LOG("SimpleAudioClip playing looped: " + filePath);
            return true;
        }
        else
        {
            DEBUG_LOG("SimpleAudioClip failed to play looped: " + filePath);
            return false;
        }
    }

    void SimpleAudioClip::Stop()
    {
        ::PlaySound(nullptr, nullptr, SND_PURGE);
        DEBUG_LOG("SimpleAudioClip stopped");
    }
}