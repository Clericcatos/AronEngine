#include "framework.h"
#include "SoundBank.h"
#include "AudioManager.h"
#include "../Resources/AudioClip.h"
#include <fstream>
#include <random>

namespace AronEngine
{
    bool SoundBankManager::LoadSoundBank(const std::string& bankName, const std::string& bankFilePath)
    {
        if (IsSoundBankLoaded(bankName))
        {
            DEBUG_LOG("SoundBank already loaded: " + bankName);
            return true;
        }

        SoundBank bank;
        bank.name = bankName;
        bank.bankPath = bankFilePath;

        if (!ParseSoundBankFile(bankFilePath, bank))
        {
            DEBUG_LOG("Failed to parse SoundBank file: " + bankFilePath);
            return false;
        }

        soundBanks[bankName] = bank;
        DEBUG_LOG("SoundBank loaded successfully: " + bankName);
        return true;
    }

    void SoundBankManager::UnloadSoundBank(const std::string& bankName)
    {
        auto it = soundBanks.find(bankName);
        if (it != soundBanks.end())
        {
            for (const auto& groupPair : it->second.soundGroups)
            {
                for (const std::string& soundPath : groupPair.second.soundPaths)
                {
                    auto clipIt = loadedClips.find(soundPath);
                    if (clipIt != loadedClips.end())
                    {
                        loadedClips.erase(clipIt);
                    }
                }
            }
            soundBanks.erase(it);
            DEBUG_LOG("SoundBank unloaded: " + bankName);
        }
    }

    void SoundBankManager::UnloadAllSoundBanks()
    {
        loadedClips.clear();
        soundBanks.clear();
        DEBUG_LOG("All SoundBanks unloaded");
    }

    std::shared_ptr<AudioClip> SoundBankManager::GetSound(const std::string& bankName, const std::string& soundName)
    {
        auto bankIt = soundBanks.find(bankName);
        if (bankIt == soundBanks.end()) return nullptr;

        auto groupIt = bankIt->second.soundGroups.find(soundName);
        if (groupIt == bankIt->second.soundGroups.end()) return nullptr;

        const SoundGroup& group = groupIt->second;
        if (group.soundPaths.empty()) return nullptr;

        const std::string& soundPath = group.soundPaths[0];
        
        auto clipIt = loadedClips.find(soundPath);
        if (clipIt != loadedClips.end())
        {
            return clipIt->second;
        }

        auto clip = AudioManager::GetInstance().LoadAudioClip(soundPath, group.is3D, group.loop, false);
        if (clip)
        {
            loadedClips[soundPath] = clip;
        }
        return clip;
    }

    std::shared_ptr<AudioClip> SoundBankManager::GetRandomSoundFromGroup(const std::string& bankName, const std::string& groupName)
    {
        auto bankIt = soundBanks.find(bankName);
        if (bankIt == soundBanks.end()) return nullptr;

        auto groupIt = bankIt->second.soundGroups.find(groupName);
        if (groupIt == bankIt->second.soundGroups.end()) return nullptr;

        const SoundGroup& group = groupIt->second;
        if (group.soundPaths.empty()) return nullptr;

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, group.soundPaths.size() - 1);
        
        const std::string& soundPath = group.soundPaths[dis(gen)];
        
        auto clipIt = loadedClips.find(soundPath);
        if (clipIt != loadedClips.end())
        {
            return clipIt->second;
        }

        auto clip = AudioManager::GetInstance().LoadAudioClip(soundPath, group.is3D, group.loop, false);
        if (clip)
        {
            loadedClips[soundPath] = clip;
        }
        return clip;
    }

    bool SoundBankManager::PlaySound(const std::string& bankName, const std::string& soundName)
    {
        auto clip = GetSound(bankName, soundName);
        if (!clip) return false;

        // Create temporary AudioSource for one-shot playback
        // In a full implementation, you'd want a SoundManager to handle this
        return true; // Placeholder - would need AudioSource integration
    }

    bool SoundBankManager::PlayRandomFromGroup(const std::string& bankName, const std::string& groupName)
    {
        auto clip = GetRandomSoundFromGroup(bankName, groupName);
        if (!clip) return false;

        // Create temporary AudioSource for one-shot playback
        return true; // Placeholder - would need AudioSource integration
    }

    SoundGroup* SoundBankManager::GetSoundGroup(const std::string& bankName, const std::string& groupName)
    {
        auto bankIt = soundBanks.find(bankName);
        if (bankIt == soundBanks.end()) return nullptr;

        auto groupIt = bankIt->second.soundGroups.find(groupName);
        if (groupIt == bankIt->second.soundGroups.end()) return nullptr;

        return &groupIt->second;
    }

    bool SoundBankManager::IsSoundBankLoaded(const std::string& bankName) const
    {
        return soundBanks.find(bankName) != soundBanks.end();
    }

    bool SoundBankManager::ParseSoundBankFile(const std::string& filePath, SoundBank& bank)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            DEBUG_LOG("Failed to open SoundBank file: " + filePath);
            return false;
        }

        std::string line;
        SoundGroup* currentGroup = nullptr;

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#') continue;

            if (line.substr(0, 6) == "GROUP:")
            {
                std::string groupName = line.substr(6);
                bank.soundGroups[groupName] = SoundGroup();
                bank.soundGroups[groupName].name = groupName;
                currentGroup = &bank.soundGroups[groupName];
            }
            else if (currentGroup && line.substr(0, 5) == "SOUND:")
            {
                std::string soundPath = line.substr(5);
                currentGroup->soundPaths.push_back(soundPath);
            }
            else if (currentGroup && line.substr(0, 7) == "VOLUME:")
            {
                currentGroup->volume = std::stof(line.substr(7));
            }
            else if (currentGroup && line.substr(0, 6) == "PITCH:")
            {
                currentGroup->pitch = std::stof(line.substr(6));
            }
            else if (currentGroup && line.substr(0, 5) == "LOOP:")
            {
                currentGroup->loop = (line.substr(5) == "true");
            }
            else if (currentGroup && line.substr(0, 3) == "3D:")
            {
                currentGroup->is3D = (line.substr(3) == "true");
            }
        }

        bank.isLoaded = true;
        return true;
    }
}