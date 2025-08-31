#pragma once
#include "Component.h"
#include <memory>
#include <algorithm>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

// Forward declarations for FMOD
namespace FMOD
{
    class Channel;
}

namespace AronEngine
{
    class AudioClip;

    class AudioSource : public Component
    {
    private:
        std::shared_ptr<AudioClip> clip;
        FMOD::Channel* channel;
        bool isPlaying;
        bool isPaused;
        bool loop;
        float volume;           // 0.0f to 1.0f
        float pitch;            // 0.1f to 3.0f, 1.0f = normal
        float spatialBlend;     // 0.0f = 2D sound, 1.0f = 3D sound
        
        // 3D Audio properties
        float minDistance;      // Minimum distance for 3D falloff
        float maxDistance;      // Maximum distance for 3D falloff
        bool mute;
        
        // Priority for audio mixing (0 = highest, 256 = lowest)
        int priority;

    public:
        AudioSource();
        virtual ~AudioSource() override;

        // Component lifecycle
        void Start() override;
        void Update(float deltaTime) override;
        void OnDestroy() override;

        // Playback control
        void Play();
        void PlayOneShot(std::shared_ptr<AudioClip> clipToPlay, float volumeScale = 1.0f);
        void Pause();
        void UnPause();
        void Stop();

        // Properties
        void SetClip(std::shared_ptr<AudioClip> newClip);
        std::shared_ptr<AudioClip> GetClip() const { return clip; }

        void SetVolume(float vol);
        float GetVolume() const { return volume; }

        void SetPitch(float p);
        float GetPitch() const { return pitch; }

        void SetLoop(bool shouldLoop);
        bool GetLoop() const { return loop; }

        void SetMute(bool shouldMute);
        bool GetMute() const { return mute; }

        void SetSpatialBlend(float blend) { spatialBlend = std::max(0.0f, std::min(1.0f, blend)); }
        float GetSpatialBlend() const { return spatialBlend; }

        void SetMinDistance(float dist) { minDistance = std::max(0.1f, dist); }
        float GetMinDistance() const { return minDistance; }

        void SetMaxDistance(float dist) { maxDistance = std::max(minDistance, dist); }
        float GetMaxDistance() const { return maxDistance; }

        void SetPriority(int prio) { priority = std::max(0, std::min(256, prio)); }
        int GetPriority() const { return priority; }

        // Status
        bool IsPlaying() const;
        bool IsPaused() const { return isPaused; }
        float GetTime() const;
        void SetTime(float time);

    private:
        void UpdateSpatialAudio();
        void ApplySettings();
    };
}