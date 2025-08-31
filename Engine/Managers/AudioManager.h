#pragma once
#include "Core/Singleton.h"
#include <memory>
#include <unordered_map>
#include <string>

// Forward declarations for FMOD
namespace FMOD
{
    class System;
    class Sound;
    class Channel;
    class ChannelGroup;
}

namespace AronEngine
{
    class AudioClip;

    class AudioManager : public Singleton<AudioManager>
    {
        friend class Singleton<AudioManager>;

    private:
        FMOD::System* fmodSystem;
        FMOD::ChannelGroup* masterGroup;
        bool initialized;
        
        // Cache loaded audio clips
        std::unordered_map<std::string, std::shared_ptr<AudioClip>> audioClips;

    protected:
        AudioManager();
        virtual ~AudioManager();

    public:
        bool Initialize(void* windowHandle = nullptr);
        void Shutdown();
        void Update(); // Call this every frame to update FMOD

        // Audio clip management
        std::shared_ptr<AudioClip> LoadAudioClip(const std::string& filePath, bool is3D = false, bool isLooping = false, bool isStream = false);
        void UnloadAudioClip(const std::string& filePath);
        void UnloadAllAudioClips();

        // FMOD system access
        FMOD::System* GetFMODSystem() const { return fmodSystem; }
        FMOD::ChannelGroup* GetMasterChannelGroup() const { return masterGroup; }

        // Volume control (0.0f to 1.0f)
        void SetMasterVolume(float volume);
        float GetMasterVolume() const;

        // Utility functions
        void Set3DListenerPosition(float x, float y, float z);
        void Set3DListenerVelocity(float vx, float vy, float vz);
        void Set3DListenerOrientation(float forwardX, float forwardY, float forwardZ, 
                                       float upX, float upY, float upZ);

    private:
        bool CreateFMODSystem();
        bool InitializeFMODSystem();
    };
}