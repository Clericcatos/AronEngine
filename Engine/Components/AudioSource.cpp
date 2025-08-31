#include "framework.h"
#include "AudioSource.h"
#include "../Resources/AudioClip.h"
#include "../Core/GameObject.h"
#include "../Components/Transform.h"
#include "../Managers/AudioManager.h"
#include "../../ThirdParty/FMOD/inc/fmod.hpp"
#include "../../ThirdParty/FMOD/inc/fmod_errors.h"

namespace AronEngine
{
    AudioSource::AudioSource()
        : Component()
        , clip(nullptr)
        , channel(nullptr)
        , isPlaying(false)
        , isPaused(false)
        , loop(false)
        , volume(1.0f)
        , pitch(1.0f)
        , spatialBlend(0.0f)    // Default to 2D sound
        , minDistance(1.0f)
        , maxDistance(500.0f)
        , mute(false)
        , priority(128)         // Medium priority
    {
    }

    AudioSource::~AudioSource()
    {
        Stop();
    }

    void AudioSource::Start()
    {
        Component::Start();
        DEBUG_LOG("AudioSource started on GameObject: " + GetGameObject()->GetName());
    }

    void AudioSource::Update(float deltaTime)
    {
        Component::Update(deltaTime);
        
        if (channel && spatialBlend > 0.0f)
        {
            UpdateSpatialAudio();
        }

        // Check if still playing
        if (channel)
        {
            bool playing = false;
            FMOD_RESULT result = channel->isPlaying(&playing);
            if (result != FMOD_OK || !playing)
            {
                isPlaying = false;
                channel = nullptr;
            }
        }
    }

    void AudioSource::OnDestroy()
    {
        Stop();
        Component::OnDestroy();
    }

    void AudioSource::Play()
    {
        if (!clip)
        {
            DEBUG_LOG("AudioSource::Play() - No audio clip assigned!");
            return;
        }

        if (!clip->IsLoaded())
        {
            DEBUG_LOG("AudioSource::Play() - Audio clip not loaded!");
            return;
        }

        FMOD::Sound* sound = clip->GetFMODSound();
        if (!sound)
        {
            DEBUG_LOG("AudioSource::Play() - No FMOD sound!");
            return;
        }

        FMOD::System* fmodSystem = AudioManager::GetInstance().GetFMODSystem();
        if (!fmodSystem)
        {
            DEBUG_LOG("AudioSource::Play() - FMOD system not initialized!");
            return;
        }

        // Stop current playback if any
        Stop();

        DEBUG_LOG("Playing audio clip: " + clip->GetFilePath());
        
        // Play the sound
        FMOD_RESULT result = fmodSystem->playSound(sound, nullptr, true, &channel);
        if (result != FMOD_OK)
        {
            DEBUG_LOG("Failed to play sound: " + std::string(FMOD_ErrorString(result)));
            return;
        }

        // Apply settings
        ApplySettings();

        // Start playback
        channel->setPaused(false);
        isPlaying = true;
        isPaused = false;
        
        DEBUG_LOG("Audio playing successfully");
    }

    void AudioSource::PlayOneShot(std::shared_ptr<AudioClip> clipToPlay, float volumeScale)
    {
        if (!clipToPlay)
        {
            DEBUG_LOG("AudioSource::PlayOneShot() - No clip provided!");
            return;
        }

        if (!clipToPlay->IsLoaded())
        {
            DEBUG_LOG("AudioSource::PlayOneShot() - Clip not loaded!");
            return;
        }

        FMOD::Sound* sound = clipToPlay->GetFMODSound();
        if (!sound)
        {
            DEBUG_LOG("AudioSource::PlayOneShot() - No FMOD sound!");
            return;
        }

        FMOD::System* fmodSystem = AudioManager::GetInstance().GetFMODSystem();
        if (!fmodSystem)
        {
            DEBUG_LOG("AudioSource::PlayOneShot() - FMOD system not initialized!");
            return;
        }

        DEBUG_LOG("Playing one-shot audio: " + clipToPlay->GetFilePath());
        
        FMOD::Channel* oneShotChannel = nullptr;
        FMOD_RESULT result = fmodSystem->playSound(sound, nullptr, true, &oneShotChannel);
        if (result != FMOD_OK)
        {
            DEBUG_LOG("Failed to play one-shot: " + std::string(FMOD_ErrorString(result)));
            return;
        }

        // Apply volume with scale
        oneShotChannel->setVolume(volume * volumeScale * (mute ? 0.0f : 1.0f));
        
        // Apply pitch
        oneShotChannel->setPitch(pitch);
        
        // Apply 3D settings if needed
        if (spatialBlend > 0.0f && clipToPlay->Is3D())
        {
            auto transform = GetGameObject()->GetComponent<Transform>();
            if (transform)
            {
                Vector2 pos = transform->GetPosition();
                FMOD_VECTOR position = { pos.x, pos.y, 0.0f };
                FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
                
                oneShotChannel->set3DAttributes(&position, &velocity);
                oneShotChannel->set3DMinMaxDistance(minDistance, maxDistance);
                
                // Mix between 2D and 3D
                float blend2D = 1.0f - spatialBlend;
                oneShotChannel->set3DLevel(spatialBlend);
            }
        }

        // Start playback
        oneShotChannel->setPaused(false);
    }

    void AudioSource::Pause()
    {
        if (channel && isPlaying && !isPaused)
        {
            channel->setPaused(true);
            isPaused = true;
            DEBUG_LOG("Audio paused");
        }
    }

    void AudioSource::UnPause()
    {
        if (channel && isPlaying && isPaused)
        {
            channel->setPaused(false);
            isPaused = false;
            DEBUG_LOG("Audio unpaused");
        }
    }

    void AudioSource::Stop()
    {
        if (channel)
        {
            channel->stop();
            channel = nullptr;
            isPlaying = false;
            isPaused = false;
            DEBUG_LOG("Audio stopped");
        }
    }

    void AudioSource::SetClip(std::shared_ptr<AudioClip> newClip)
    {
        // Stop current audio if playing
        if (isPlaying)
        {
            Stop();
        }

        clip = newClip;
        
        if (clip)
        {
            DEBUG_LOG("AudioClip assigned: " + clip->GetFilePath());
        }
        else
        {
            DEBUG_LOG("AudioClip cleared");
        }
    }

    void AudioSource::SetVolume(float vol)
    {
        volume = std::max(0.0f, std::min(1.0f, vol));
        if (channel)
        {
            channel->setVolume(volume * (mute ? 0.0f : 1.0f));
        }
    }

    void AudioSource::SetPitch(float p)
    {
        pitch = std::max(0.1f, std::min(3.0f, p));
        if (channel)
        {
            channel->setPitch(pitch);
        }
    }

    void AudioSource::SetLoop(bool shouldLoop)
    {
        loop = shouldLoop;
        if (channel)
        {
            channel->setMode(shouldLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        }
    }

    void AudioSource::SetMute(bool shouldMute)
    {
        mute = shouldMute;
        if (channel)
        {
            channel->setVolume(volume * (mute ? 0.0f : 1.0f));
        }
    }

    bool AudioSource::IsPlaying() const
    {
        if (!channel) return false;
        
        bool playing = false;
        FMOD_RESULT result = channel->isPlaying(&playing);
        return (result == FMOD_OK) && playing;
    }

    float AudioSource::GetTime() const
    {
        if (!channel) return 0.0f;
        
        unsigned int position = 0;
        FMOD_RESULT result = channel->getPosition(&position, FMOD_TIMEUNIT_MS);
        if (result == FMOD_OK)
        {
            return position / 1000.0f; // Convert to seconds
        }
        return 0.0f;
    }

    void AudioSource::SetTime(float time)
    {
        if (!channel) return;
        
        unsigned int position = static_cast<unsigned int>(time * 1000.0f); // Convert to milliseconds
        channel->setPosition(position, FMOD_TIMEUNIT_MS);
    }

    void AudioSource::UpdateSpatialAudio()
    {
        if (!channel) return;
        
        // Get position from Transform component
        auto transform = GetGameObject()->GetComponent<Transform>();
        if (!transform) return;

        Vector2 position = transform->GetPosition();
        
        // Update 3D position
        FMOD_VECTOR pos = { position.x, position.y, 0.0f };
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f }; // TODO: Calculate velocity if needed
        
        channel->set3DAttributes(&pos, &vel);
    }

    void AudioSource::ApplySettings()
    {
        if (!channel) return;

        // Volume and mute
        channel->setVolume(volume * (mute ? 0.0f : 1.0f));
        
        // Pitch
        channel->setPitch(pitch);
        
        // Loop
        channel->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        
        // Priority
        channel->setPriority(priority);
        
        // 3D settings
        if (clip && clip->Is3D() && spatialBlend > 0.0f)
        {
            channel->set3DMinMaxDistance(minDistance, maxDistance);
            channel->set3DLevel(spatialBlend);
            
            // Update position immediately
            UpdateSpatialAudio();
        }
    }
}