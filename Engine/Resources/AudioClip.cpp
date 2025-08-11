#include "framework.h"
#include "AudioClip.h"
#include "../Managers/AudioManager.h"
#include <fstream>
#include <dsound.h>
#include <mmsystem.h>
#include <mmreg.h>

#pragma comment(lib, "winmm.lib")

namespace AronEngine
{
    AudioClip::AudioClip()
        : filePath("")
        , format(AudioFormat::WAV)
        , soundBuffer(nullptr)
        , length(0.0f)
        , frequency(44100)
        , channels(2)
        , isLoaded(false)
    {
    }

    AudioClip::AudioClip(const std::string& path)
        : AudioClip()
    {
        LoadFromFile(path);
    }

    AudioClip::~AudioClip()
    {
        Unload();
    }

    bool AudioClip::LoadFromFile(const std::string& path)
    {
        filePath = path;
        format = DetectFormat(path);

        switch (format)
        {
        case AudioFormat::WAV:
            return LoadWAV(path);
        case AudioFormat::MP3:
        case AudioFormat::OGG:
            DEBUG_LOG("MP3/OGG support not implemented yet, use WAV files");
            return false;
        }
        return false;
    }

    void AudioClip::Unload()
    {
        if (soundBuffer)
        {
            soundBuffer->Release();
            soundBuffer = nullptr;
        }
        isLoaded = false;
    }

    bool AudioClip::LoadWAV(const std::string& path)
    {
        DEBUG_LOG("Loading WAV file: " + path);
        
        // Get AudioManager for DirectSound
        auto audioManager = AudioManager::GetInstance().GetDirectSound();
        if (!audioManager)
        {
            DEBUG_LOG("AudioManager not initialized");
            return false;
        }

        // Open WAV file
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            DEBUG_LOG("Failed to open WAV file: " + path);
            return false;
        }

        // Read WAV header
        struct WAVHeader
        {
            char riff[4];           // "RIFF"
            uint32_t fileSize;      // File size - 8
            char wave[4];           // "WAVE"
            char fmt[4];            // "fmt "
            uint32_t fmtSize;       // Format chunk size
            uint16_t audioFormat;   // Audio format (1 = PCM)
            uint16_t numChannels;   // Number of channels
            uint32_t sampleRate;    // Sample rate
            uint32_t byteRate;      // Byte rate
            uint16_t blockAlign;    // Block align
            uint16_t bitsPerSample; // Bits per sample
            char data[4];           // "data"
            uint32_t dataSize;      // Data chunk size
        };

        WAVHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));

        if (strncmp(header.riff, "RIFF", 4) != 0 || strncmp(header.wave, "WAVE", 4) != 0)
        {
            DEBUG_LOG("Invalid WAV file format: " + path);
            file.close();
            return false;
        }

        // Set audio properties
        frequency = header.sampleRate;
        channels = header.numChannels;
        length = static_cast<float>(header.dataSize) / header.byteRate;

        // Create WAVEFORMATEX structure
        WAVEFORMATEX waveFormat = {};
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = header.numChannels;
        waveFormat.nSamplesPerSec = header.sampleRate;
        waveFormat.wBitsPerSample = header.bitsPerSample;
        waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
        waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

        // Create DirectSound buffer description
        DSBUFFERDESC bufferDesc = {};
        bufferDesc.dwSize = sizeof(DSBUFFERDESC);
        bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_STATIC;
        bufferDesc.dwBufferBytes = header.dataSize;
        bufferDesc.lpwfxFormat = &waveFormat;

        // Create DirectSound buffer
        IDirectSoundBuffer* tempBuffer = nullptr;
        HRESULT result = audioManager->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
        if (FAILED(result))
        {
            DEBUG_LOG("Failed to create DirectSound buffer for: " + path);
            file.close();
            return false;
        }

        // Query for IDirectSoundBuffer8
        result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, reinterpret_cast<void**>(&soundBuffer));
        tempBuffer->Release();
        
        if (FAILED(result))
        {
            DEBUG_LOG("Failed to get DirectSoundBuffer8 interface for: " + path);
            file.close();
            return false;
        }

        // Read WAV data
        std::vector<char> audioData(header.dataSize);
        file.read(audioData.data(), header.dataSize);
        file.close();

        // Lock buffer and copy data
        void* bufferPtr1 = nullptr;
        void* bufferPtr2 = nullptr;
        DWORD bufferSize1 = 0;
        DWORD bufferSize2 = 0;

        result = soundBuffer->Lock(0, header.dataSize, &bufferPtr1, &bufferSize1, &bufferPtr2, &bufferSize2, DSBLOCK_ENTIREBUFFER);
        if (SUCCEEDED(result))
        {
            memcpy(bufferPtr1, audioData.data(), bufferSize1);
            if (bufferPtr2 != nullptr)
            {
                memcpy(bufferPtr2, audioData.data() + bufferSize1, bufferSize2);
            }
            soundBuffer->Unlock(bufferPtr1, bufferSize1, bufferPtr2, bufferSize2);
            
            isLoaded = true;
            DEBUG_LOG("WAV loaded successfully: " + path + " (" + 
                     std::to_string(length) + "s, " + 
                     std::to_string(frequency) + "Hz, " + 
                     std::to_string(channels) + " channels)");
            return true;
        }
        else
        {
            DEBUG_LOG("Failed to lock DirectSound buffer for: " + path);
            soundBuffer->Release();
            soundBuffer = nullptr;
            return false;
        }
    }

    AudioFormat AudioClip::DetectFormat(const std::string& path)
    {
        std::string extension = path.substr(path.find_last_of(".") + 1);
        
        if (extension == "wav" || extension == "WAV")
            return AudioFormat::WAV;
        else if (extension == "mp3" || extension == "MP3")
            return AudioFormat::MP3;
        else if (extension == "ogg" || extension == "OGG")
            return AudioFormat::OGG;
        
        return AudioFormat::WAV; // Default
    }
}