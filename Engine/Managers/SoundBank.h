#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace AronEngine
{
    class AudioClip;

    struct SoundGroup
    {
        std::string name;
        std::vector<std::string> soundPaths;
        float volume;
        float pitch;
        bool loop;
        bool is3D;
        
        SoundGroup() : volume(1.0f), pitch(1.0f), loop(false), is3D(false) {}
    };

    struct SoundBank
    {
        std::string name;
        std::string bankPath;
        std::unordered_map<std::string, SoundGroup> soundGroups;
        bool isLoaded;

        SoundBank() : isLoaded(false) {}
    };

    class SoundBankManager
    {
    private:
        std::unordered_map<std::string, SoundBank> soundBanks;
        std::unordered_map<std::string, std::shared_ptr<AudioClip>> loadedClips;

    public:
        SoundBankManager() = default;
        ~SoundBankManager() = default;

        bool LoadSoundBank(const std::string& bankName, const std::string& bankFilePath);
        void UnloadSoundBank(const std::string& bankName);
        void UnloadAllSoundBanks();

        std::shared_ptr<AudioClip> GetSound(const std::string& bankName, const std::string& soundName);
        std::shared_ptr<AudioClip> GetRandomSoundFromGroup(const std::string& bankName, const std::string& groupName);
        
        bool PlaySound(const std::string& bankName, const std::string& soundName);
        bool PlayRandomFromGroup(const std::string& bankName, const std::string& groupName);

        SoundGroup* GetSoundGroup(const std::string& bankName, const std::string& groupName);
        bool IsSoundBankLoaded(const std::string& bankName) const;

    private:
        bool ParseSoundBankFile(const std::string& filePath, SoundBank& bank);
    };
}