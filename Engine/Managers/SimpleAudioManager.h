#pragma once
#include "Core/Singleton.h"
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <unordered_map>
#include <memory>

#pragma comment(lib, "winmm.lib")

namespace AronEngine
{
    class SimpleAudioClip;

    class SimpleAudioManager : public Singleton<SimpleAudioManager>
    {
        friend class Singleton<SimpleAudioManager>;

    private:
        bool initialized;
        std::unordered_map<std::string, std::shared_ptr<SimpleAudioClip>> audioClips;

    protected:
        SimpleAudioManager();
        virtual ~SimpleAudioManager();

    public:
        bool Initialize();
        void Shutdown();

        // Audio clip management
        std::shared_ptr<SimpleAudioClip> LoadAudioClip(const std::string& filePath);
        void UnloadAudioClip(const std::string& filePath);
        void UnloadAllAudioClips();

        // Simple playback functions
        bool PlaySound(const std::string& filePath, bool async = true);
        bool PlaySoundLoop(const std::string& filePath);
        void StopAllSounds();

        bool IsInitialized() const { return initialized; }
    };

    // Simple audio clip using Windows PlaySound
    class SimpleAudioClip
    {
    private:
        std::string filePath;
        bool isLoaded;
        float length;

    public:
        SimpleAudioClip();
        SimpleAudioClip(const std::string& path);
        virtual ~SimpleAudioClip();

        bool LoadFromFile(const std::string& path);
        void Unload();

        // Playback
        bool Play(bool async = true);
        bool PlayLoop();
        void Stop();

        // Properties
        bool IsLoaded() const { return isLoaded; }
        const std::string& GetFilePath() const { return filePath; }
        float GetLength() const { return length; }
    };
}