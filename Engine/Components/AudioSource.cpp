#include "framework.h"
#include "AudioSource.h"
#include "../Resources/AudioClip.h"
#include "../Core/GameObject.h"
#include "../Components/Transform.h"

namespace AronEngine
{
    AudioSource::AudioSource()
        : Component()
        , clip(nullptr)
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
        
        if (isPlaying && !isPaused && spatialBlend > 0.0f)
        {
            UpdateSpatialAudio();
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

        auto soundBuffer = clip->GetSoundBuffer();
        if (!soundBuffer)
        {
            DEBUG_LOG("AudioSource::Play() - No sound buffer!");
            return;
        }

        DEBUG_LOG("Playing audio clip: " + clip->GetFilePath());
        
        // Set volume
        long attenuation = static_cast<long>(2000.0f * log10f(std::max(0.001f, volume * (mute ? 0.0f : 1.0f))));
        attenuation = std::max(DSBVOLUME_MIN, std::min(DSBVOLUME_MAX, attenuation));
        soundBuffer->SetVolume(attenuation);

        // Set looping
        DWORD playFlags = loop ? DSBPLAY_LOOPING : 0;
        
        // Play the sound
        HRESULT result = soundBuffer->Play(0, 0, playFlags);
        if (SUCCEEDED(result))
        {
            isPlaying = true;
            isPaused = false;
            DEBUG_LOG("Audio playing successfully");
        }
        else
        {
            DEBUG_LOG("Failed to play audio clip");
        }
    }

    void AudioSource::PlayOneShot(std::shared_ptr<AudioClip> clipToPlay)
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

        DEBUG_LOG("Playing one-shot audio: " + clipToPlay->GetFilePath());
        
        // TODO: Implement one-shot playback that doesn't affect main clip
        // This should play the sound once without changing the current clip
    }

    void AudioSource::Pause()
    {
        if (isPlaying && !isPaused && clip && clip->GetSoundBuffer())
        {
            clip->GetSoundBuffer()->Stop();
            isPaused = true;
            DEBUG_LOG("Audio paused");
        }
    }

    void AudioSource::UnPause()
    {
        if (isPlaying && isPaused && clip && clip->GetSoundBuffer())
        {
            DWORD playFlags = loop ? DSBPLAY_LOOPING : 0;
            clip->GetSoundBuffer()->Play(0, 0, playFlags);
            isPaused = false;
            DEBUG_LOG("Audio unpaused");
        }
    }

    void AudioSource::Stop()
    {
        if (isPlaying && clip && clip->GetSoundBuffer())
        {
            clip->GetSoundBuffer()->Stop();
            clip->GetSoundBuffer()->SetCurrentPosition(0);
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

    void AudioSource::UpdateSpatialAudio()
    {
        // Get position from Transform component
        auto transform = GetGameObject()->GetComponent<Transform>();
        if (!transform) return;

        Vector2 position = transform->GetPosition();
        
        // TODO: Implement 3D audio positioning using DirectSound3D
        // Calculate distance to listener, apply volume falloff, etc.
        
        // For now, just log the position update
        static int frameCount = 0;
        if (frameCount++ % 60 == 0)  // Log every 60 frames to avoid spam
        {
            DEBUG_LOG("3D Audio position update: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
        }
    }
}