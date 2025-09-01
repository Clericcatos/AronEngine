#pragma once
#include "Component.h"
#include <string>
#include <memory>

namespace FMOD
{
    class Channel;
}

namespace AronEngine
{
    class AudioClip;

    class SoundComponent : public Component
    {
    private:
        std::shared_ptr<AudioClip> clip;
        FMOD::Channel* channel;
        
        std::string soundBankName;
        std::string soundGroupName;
        
        bool isPlaying;
        bool isPaused;
        bool autoPlay;
        bool loop;
        bool fadeIn;
        bool fadeOut;
        
        float volume;
        float pitch;
        float spatialBlend;
        float minDistance;
        float maxDistance;
        int priority;
        
        float fadeInDuration;
        float fadeOutDuration;
        float currentFadeTime;
        float targetVolume;
        float startVolume;
        bool isFading;
        
        bool mute;

    public:
        SoundComponent();
        virtual ~SoundComponent() override;

        void Start() override;
        void Update(float deltaTime) override;
        void OnDestroy() override;

        void Play();
        void PlayFromBank(const std::string& bankName, const std::string& soundName);
        void PlayRandomFromGroup(const std::string& bankName, const std::string& groupName);
        void Pause();
        void UnPause();
        void Stop();
        void StopWithFadeOut(float fadeTime = 1.0f);

        void FadeIn(float duration = 1.0f);
        void FadeOut(float duration = 1.0f);
        void FadeToVolume(float targetVol, float duration = 1.0f);

        void SetClip(std::shared_ptr<AudioClip> newClip);
        std::shared_ptr<AudioClip> GetClip() const { return clip; }

        void SetSoundBank(const std::string& bankName, const std::string& groupName);
        
        void SetVolume(float vol);
        float GetVolume() const { return volume; }
        
        void SetPitch(float p);
        float GetPitch() const { return pitch; }
        
        void SetLoop(bool shouldLoop);
        bool GetLoop() const { return loop; }
        
        void SetAutoPlay(bool shouldAutoPlay) { autoPlay = shouldAutoPlay; }
        bool GetAutoPlay() const { return autoPlay; }
        
        void SetMute(bool shouldMute);
        bool GetMute() const { return mute; }
        
        void SetSpatialBlend(float blend);
        float GetSpatialBlend() const { return spatialBlend; }
        
        void SetMinDistance(float dist);
        float GetMinDistance() const { return minDistance; }
        
        void SetMaxDistance(float dist);
        float GetMaxDistance() const { return maxDistance; }
        
        void SetPriority(int prio);
        int GetPriority() const { return priority; }

        bool IsPlaying() const;
        bool IsPaused() const { return isPaused; }
        bool IsFading() const { return isFading; }
        
        float GetTime() const;
        void SetTime(float time);

    private:
        void UpdateSpatialAudio();
        void UpdateFading(float deltaTime);
        void ApplySettings();
    };
}