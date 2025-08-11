#pragma once
#include "Core/Singleton.h"
#include <dsound.h>
#include <memory>
#include <unordered_map>
#include <string>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

namespace AronEngine
{
    class AudioClip;

    class AudioManager : public Singleton<AudioManager>
    {
        friend class Singleton<AudioManager>;

    private:
        IDirectSound8* directSound;
        IDirectSoundBuffer* primaryBuffer;
        HWND hWnd;
        bool initialized;

        // Cache loaded audio clips
        std::unordered_map<std::string, std::shared_ptr<AudioClip>> audioClips;

    protected:
        AudioManager();
        virtual ~AudioManager();

    public:
        bool Initialize(HWND windowHandle);
        void Shutdown();

        // Audio clip management
        std::shared_ptr<AudioClip> LoadAudioClip(const std::string& filePath);
        void UnloadAudioClip(const std::string& filePath);
        void UnloadAllAudioClips();

        // DirectSound access
        IDirectSound8* GetDirectSound() const { return directSound; }

        // Volume control (0.0f to 1.0f)
        void SetMasterVolume(float volume);
        float GetMasterVolume() const;

        // Audio format support check
        bool IsFormatSupported(const WAVEFORMATEX& format) const;

    private:
        bool CreateDirectSound();
        bool SetCooperativeLevel();
        bool CreatePrimaryBuffer();
    };

    // Helper function to convert volume (0.0f to 1.0f) to DirectSound attenuation
    long VolumeToAttenuation(float volume);
    float AttenuationToVolume(long attenuation);
}