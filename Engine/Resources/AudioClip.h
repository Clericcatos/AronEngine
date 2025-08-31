#pragma once
#include "Core/Object.h"
#include <string>
#include <memory>

// Forward declarations for FMOD
namespace FMOD
{
    class Sound;
    class Channel;
    class System;
}

namespace AronEngine
{
    enum class AudioFormat
    {
        WAV,
        MP3,
        OGG,
        FLAC,
        UNKNOWN
    };

    class AudioClip : public Object
    {
    private:
        std::string filePath;
        AudioFormat format;
        FMOD::Sound* fmodSound;
        
        // Audio properties
        float length;           // Duration in seconds
        int frequency;          // Sample rate
        int channels;           // Mono=1, Stereo=2
        bool isLoaded;
        bool is3D;
        bool isLooping;
        bool isStream;

    public:
        AudioClip();
        AudioClip(const std::string& path);
        virtual ~AudioClip();

        DECLARE_OBJECT_TYPE(AudioClip)

        // Loading
        bool LoadFromFile(const std::string& path, bool is3D = false, bool isLooping = false, bool isStream = false);
        void Unload();

        // Properties
        float GetLength() const { return length; }
        int GetFrequency() const { return frequency; }
        int GetChannels() const { return channels; }
        bool IsLoaded() const { return isLoaded; }
        bool Is3D() const { return is3D; }
        bool IsLooping() const { return isLooping; }
        bool IsStream() const { return isStream; }
        const std::string& GetFilePath() const { return filePath; }
        AudioFormat GetFormat() const { return format; }

        // FMOD access
        FMOD::Sound* GetFMODSound() const { return fmodSound; }

    private:
        AudioFormat DetectFormat(const std::string& path);
        void UpdateAudioProperties();
    };
}