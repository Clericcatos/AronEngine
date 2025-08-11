#pragma once
#include "Core/Object.h"
#include <string>
#include <memory>
#include <dsound.h>

namespace AronEngine
{
    enum class AudioFormat
    {
        WAV,
        MP3,
        OGG
    };

    class AudioClip : public Object
    {
    private:
        std::string filePath;
        AudioFormat format;
        IDirectSoundBuffer8* soundBuffer;
        
        // Audio properties
        float length;           // Duration in seconds
        int frequency;          // Sample rate
        int channels;           // Mono=1, Stereo=2
        bool isLoaded;

    public:
        AudioClip();
        AudioClip(const std::string& path);
        virtual ~AudioClip();

        // Loading
        bool LoadFromFile(const std::string& path);
        void Unload();

        // Properties
        float GetLength() const { return length; }
        int GetFrequency() const { return frequency; }
        int GetChannels() const { return channels; }
        bool IsLoaded() const { return isLoaded; }
        const std::string& GetFilePath() const { return filePath; }
        AudioFormat GetFormat() const { return format; }

        // DirectSound access
        IDirectSoundBuffer8* GetSoundBuffer() const { return soundBuffer; }

    private:
        bool LoadWAV(const std::string& path);
        AudioFormat DetectFormat(const std::string& path);
    };
}