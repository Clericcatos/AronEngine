#include "framework.h"
#include "AudioManager.h"
#include "../Resources/AudioClip.h"
#include <mmsystem.h>
#include <mmreg.h>

namespace AronEngine
{
    AudioManager::AudioManager()
        : directSound(nullptr)
        , primaryBuffer(nullptr)
        , hWnd(nullptr)
        , initialized(false)
    {
    }

    AudioManager::~AudioManager()
    {
        Shutdown();
    }

    bool AudioManager::Initialize(HWND windowHandle)
    {
        if (initialized) return true;

        hWnd = windowHandle;

        if (!CreateDirectSound())
        {
            DEBUG_LOG("Failed to create DirectSound");
            return false;
        }

        if (!SetCooperativeLevel())
        {
            DEBUG_LOG("Failed to set DirectSound cooperative level");
            return false;
        }

        if (!CreatePrimaryBuffer())
        {
            DEBUG_LOG("Failed to create primary buffer");
            return false;
        }

        initialized = true;
        DEBUG_LOG("AudioManager initialized successfully with DirectSound");
        return true;
    }

    void AudioManager::Shutdown()
    {
        if (!initialized) return;

        UnloadAllAudioClips();

        if (primaryBuffer)
        {
            primaryBuffer->Release();
            primaryBuffer = nullptr;
        }

        if (directSound)
        {
            directSound->Release();
            directSound = nullptr;
        }

        initialized = false;
        DEBUG_LOG("AudioManager shutdown");
    }

    std::shared_ptr<AudioClip> AudioManager::LoadAudioClip(const std::string& filePath)
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
        auto audioClip = std::make_shared<AudioClip>();
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

    void AudioManager::UnloadAudioClip(const std::string& filePath)
    {
        auto it = audioClips.find(filePath);
        if (it != audioClips.end())
        {
            it->second->Unload();
            audioClips.erase(it);
            DEBUG_LOG("Audio clip unloaded: " + filePath);
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
        if (!primaryBuffer) return;

        // Clamp volume between 0.0f and 1.0f
        volume = std::max(0.0f, std::min(1.0f, volume));
        
        long attenuation = VolumeToAttenuation(volume);
        HRESULT result = primaryBuffer->SetVolume(attenuation);
        
        if (SUCCEEDED(result))
        {
            DEBUG_LOG("Master volume set to: " + std::to_string(volume));
        }
        else
        {
            DEBUG_LOG("Failed to set master volume");
        }
    }

    float AudioManager::GetMasterVolume() const
    {
        if (!primaryBuffer) return 0.0f;

        long attenuation;
        HRESULT result = primaryBuffer->GetVolume(&attenuation);
        
        if (SUCCEEDED(result))
        {
            return AttenuationToVolume(attenuation);
        }
        
        return 0.0f;
    }

    bool AudioManager::IsFormatSupported(const WAVEFORMATEX& format) const
    {
        if (!directSound) return false;

        // Try to create a temporary buffer with this format
        DSBUFFERDESC bufferDesc = {};
        bufferDesc.dwSize = sizeof(DSBUFFERDESC);
        bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
        bufferDesc.dwBufferBytes = format.nAvgBytesPerSec; // 1 second buffer
        bufferDesc.lpwfxFormat = const_cast<WAVEFORMATEX*>(&format);

        IDirectSoundBuffer* tempBuffer = nullptr;
        HRESULT result = directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
        
        if (SUCCEEDED(result))
        {
            tempBuffer->Release();
            return true;
        }
        
        return false;
    }

    bool AudioManager::CreateDirectSound()
    {
        HRESULT result = DirectSoundCreate8(nullptr, &directSound, nullptr);
        return SUCCEEDED(result);
    }

    bool AudioManager::SetCooperativeLevel()
    {
        if (!directSound || !hWnd) return false;

        HRESULT result = directSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
        return SUCCEEDED(result);
    }

    bool AudioManager::CreatePrimaryBuffer()
    {
        if (!directSound) return false;

        DSBUFFERDESC bufferDesc = {};
        bufferDesc.dwSize = sizeof(DSBUFFERDESC);
        bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
        bufferDesc.dwBufferBytes = 0;
        bufferDesc.lpwfxFormat = nullptr;

        HRESULT result = directSound->CreateSoundBuffer(&bufferDesc, &primaryBuffer, nullptr);
        
        if (SUCCEEDED(result))
        {
            // Set primary buffer format to 44.1kHz, 16-bit, stereo
            WAVEFORMATEX waveFormat = {};
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = 2;
            waveFormat.nSamplesPerSec = 44100;
            waveFormat.wBitsPerSample = 16;
            waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            
            result = primaryBuffer->SetFormat(&waveFormat);
            return SUCCEEDED(result);
        }

        return false;
    }

    // Helper functions
    long VolumeToAttenuation(float volume)
    {
        if (volume <= 0.0f)
            return DSBVOLUME_MIN;
        if (volume >= 1.0f)
            return DSBVOLUME_MAX;
        
        // Convert linear volume to logarithmic attenuation (in centibels)
        // DirectSound uses attenuation from 0 (loudest) to -10000 (silence)
        return static_cast<long>(2000.0f * log10f(volume));
    }

    float AttenuationToVolume(long attenuation)
    {
        if (attenuation <= DSBVOLUME_MIN)
            return 0.0f;
        if (attenuation >= DSBVOLUME_MAX)
            return 1.0f;
            
        // Convert logarithmic attenuation back to linear volume
        return powf(10.0f, attenuation / 2000.0f);
    }
}