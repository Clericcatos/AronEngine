#include "framework.h"
#include "SoundComponent.h"
#include "../Resources/AudioClip.h"
#include "../Core/GameObject.h"
#include "../Components/Transform.h"
#include "../Managers/AudioManager.h"
#include "../../ThirdParty/FMOD/inc/fmod.hpp"
#include "../../ThirdParty/FMOD/inc/fmod_errors.h"
#include <algorithm>

namespace AronEngine
{
    SoundComponent::SoundComponent()
        : Component("SoundComponent")
        , clip(nullptr)
        , channel(nullptr)
        , isPlaying(false)
        , isPaused(false)
        , autoPlay(false)
        , loop(false)
        , fadeIn(false)
        , fadeOut(false)
        , volume(1.0f)
        , pitch(1.0f)
        , spatialBlend(0.0f)
        , minDistance(1.0f)
        , maxDistance(500.0f)
        , priority(128)
        , fadeInDuration(1.0f)
        , fadeOutDuration(1.0f)
        , currentFadeTime(0.0f)
        , targetVolume(1.0f)
        , startVolume(0.0f)
        , isFading(false)
        , mute(false)
    {
    }

    SoundComponent::~SoundComponent()
    {
        Stop();
    }

    void SoundComponent::Start()
    {
        Component::Start();
        
        if (autoPlay && clip)
        {
            Play();
        }
    }

    void SoundComponent::Update(float deltaTime)
    {
        Component::Update(deltaTime);
        
        if (isFading)
        {
            UpdateFading(deltaTime);
        }
        
        if (channel && spatialBlend > 0.0f)
        {
            UpdateSpatialAudio();
        }

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

    void SoundComponent::OnDestroy()
    {
        Stop();
        Component::OnDestroy();
    }

    void SoundComponent::Play()
    {
        if (!clip)
        {
            DEBUG_LOG("SoundComponent::Play() - No audio clip assigned!");
            return;
        }

        if (!clip->IsLoaded())
        {
            DEBUG_LOG("SoundComponent::Play() - Audio clip not loaded!");
            return;
        }

        FMOD::Sound* sound = clip->GetFMODSound();
        if (!sound) return;

        FMOD::System* fmodSystem = AudioManager::GetInstance().GetFMODSystem();
        if (!fmodSystem) return;

        Stop();

        FMOD_RESULT result = fmodSystem->playSound(sound, nullptr, true, &channel);
        if (result != FMOD_OK)
        {
            DEBUG_LOG("Failed to play sound: " + std::string(FMOD_ErrorString(result)));
            return;
        }

        ApplySettings();
        
        if (fadeIn)
        {
            FadeIn(fadeInDuration);
        }
        else
        {
            channel->setPaused(false);
        }
        
        isPlaying = true;
        isPaused = false;
    }

    void SoundComponent::PlayFromBank(const std::string& bankName, const std::string& soundName)
    {
        soundBankName = bankName;
        soundGroupName = soundName;
        // TODO: Integrate with SoundBankManager when available
        DEBUG_LOG("PlayFromBank: " + bankName + "/" + soundName);
    }

    void SoundComponent::PlayRandomFromGroup(const std::string& bankName, const std::string& groupName)
    {
        soundBankName = bankName;
        soundGroupName = groupName;
        // TODO: Integrate with SoundBankManager when available
        DEBUG_LOG("PlayRandomFromGroup: " + bankName + "/" + groupName);
    }

    void SoundComponent::Pause()
    {
        if (channel && isPlaying && !isPaused)
        {
            channel->setPaused(true);
            isPaused = true;
        }
    }

    void SoundComponent::UnPause()
    {
        if (channel && isPlaying && isPaused)
        {
            channel->setPaused(false);
            isPaused = false;
        }
    }

    void SoundComponent::Stop()
    {
        if (channel)
        {
            channel->stop();
            channel = nullptr;
            isPlaying = false;
            isPaused = false;
            isFading = false;
        }
    }

    void SoundComponent::StopWithFadeOut(float fadeTime)
    {
        if (channel && isPlaying)
        {
            FadeOut(fadeTime);
        }
    }

    void SoundComponent::FadeIn(float duration)
    {
        if (!channel) return;
        
        fadeInDuration = duration;
        currentFadeTime = 0.0f;
        startVolume = 0.0f;
        targetVolume = volume;
        isFading = true;
        
        channel->setVolume(0.0f);
        channel->setPaused(false);
    }

    void SoundComponent::FadeOut(float duration)
    {
        if (!channel) return;
        
        fadeOutDuration = duration;
        currentFadeTime = 0.0f;
        
        float currentVol = volume;
        startVolume = currentVol;
        targetVolume = 0.0f;
        isFading = true;
        fadeOut = true;
    }

    void SoundComponent::FadeToVolume(float targetVol, float duration)
    {
        if (!channel) return;
        
        fadeInDuration = duration;
        currentFadeTime = 0.0f;
        
        float currentVol = volume;
        startVolume = currentVol;
        targetVolume = targetVol;
        isFading = true;
    }

    void SoundComponent::SetClip(std::shared_ptr<AudioClip> newClip)
    {
        if (isPlaying) Stop();
        clip = newClip;
    }

    void SoundComponent::SetSoundBank(const std::string& bankName, const std::string& groupName)
    {
        soundBankName = bankName;
        soundGroupName = groupName;
    }

    void SoundComponent::SetVolume(float vol)
    {
        volume = std::max(0.0f, std::min(1.0f, vol));
        if (channel && !isFading)
        {
            channel->setVolume(volume * (mute ? 0.0f : 1.0f));
        }
    }

    void SoundComponent::SetPitch(float p)
    {
        pitch = std::max(0.1f, std::min(3.0f, p));
        if (channel)
        {
            channel->setPitch(pitch);
        }
    }

    void SoundComponent::SetLoop(bool shouldLoop)
    {
        loop = shouldLoop;
        if (channel)
        {
            channel->setMode(shouldLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        }
    }

    void SoundComponent::SetMute(bool shouldMute)
    {
        mute = shouldMute;
        if (channel && !isFading)
        {
            channel->setVolume(volume * (mute ? 0.0f : 1.0f));
        }
    }

    void SoundComponent::SetSpatialBlend(float blend)
    {
        spatialBlend = std::max(0.0f, std::min(1.0f, blend));
        if (channel)
        {
            channel->set3DLevel(spatialBlend);
        }
    }

    void SoundComponent::SetMinDistance(float dist)
    {
        minDistance = std::max(0.1f, dist);
        if (channel)
        {
            channel->set3DMinMaxDistance(minDistance, maxDistance);
        }
    }

    void SoundComponent::SetMaxDistance(float dist)
    {
        maxDistance = std::max(minDistance, dist);
        if (channel)
        {
            channel->set3DMinMaxDistance(minDistance, maxDistance);
        }
    }

    void SoundComponent::SetPriority(int prio)
    {
        priority = std::max(0, std::min(256, prio));
        if (channel)
        {
            channel->setPriority(priority);
        }
    }

    bool SoundComponent::IsPlaying() const
    {
        if (!channel) return false;
        
        bool playing = false;
        FMOD_RESULT result = channel->isPlaying(&playing);
        return (result == FMOD_OK) && playing;
    }

    float SoundComponent::GetTime() const
    {
        if (!channel) return 0.0f;
        
        unsigned int position = 0;
        FMOD_RESULT result = channel->getPosition(&position, FMOD_TIMEUNIT_MS);
        if (result == FMOD_OK)
        {
            return position / 1000.0f;
        }
        return 0.0f;
    }

    void SoundComponent::SetTime(float time)
    {
        if (!channel) return;
        
        unsigned int position = static_cast<unsigned int>(time * 1000.0f);
        channel->setPosition(position, FMOD_TIMEUNIT_MS);
    }

    void SoundComponent::UpdateSpatialAudio()
    {
        if (!channel) return;
        
        auto transform = GetGameObject()->GetComponent<Transform>();
        if (!transform) return;

        Vector2 position = transform->GetPosition();
        
        FMOD_VECTOR pos = { position.x, position.y, 0.0f };
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
        
        channel->set3DAttributes(&pos, &vel);
    }

    void SoundComponent::UpdateFading(float deltaTime)
    {
        if (!isFading || !channel) return;
        
        currentFadeTime += deltaTime;
        float duration = fadeOut ? fadeOutDuration : fadeInDuration;
        
        if (currentFadeTime >= duration)
        {
            currentFadeTime = duration;
            isFading = false;
            
            if (fadeOut)
            {
                Stop();
                fadeOut = false;
                return;
            }
        }
        
        float t = currentFadeTime / duration;
        float currentVolume = startVolume + (targetVolume - startVolume) * t;
        
        channel->setVolume(currentVolume * (mute ? 0.0f : 1.0f));
    }

    void SoundComponent::ApplySettings()
    {
        if (!channel) return;

        channel->setVolume(volume * (mute ? 0.0f : 1.0f));
        channel->setPitch(pitch);
        channel->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        channel->setPriority(priority);
        
        if (clip && clip->Is3D() && spatialBlend > 0.0f)
        {
            channel->set3DMinMaxDistance(minDistance, maxDistance);
            channel->set3DLevel(spatialBlend);
            UpdateSpatialAudio();
        }
    }
}