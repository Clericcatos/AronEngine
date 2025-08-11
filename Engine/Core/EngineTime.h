#pragma once
#include "framework.h"

namespace AronEngine
{
    class Time
    {
    private:
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point lastFrameTime;
        std::chrono::steady_clock::time_point currentFrameTime;
        
        float deltaTime;
        float timeScale;
        float totalTime;
        float fps;
        float frameTime;
        
        int frameCount;
        float fpsUpdateTime;

    public:
        Time();
        ~Time() = default;

        void Start();
        void Update();

        float GetDeltaTime() const { return deltaTime * timeScale; }
        float GetUnscaledDeltaTime() const { return deltaTime; }
        float GetTimeScale() const { return timeScale; }
        void SetTimeScale(float scale) { timeScale = scale; }

        float GetTotalTime() const { return totalTime; }
        float GetFPS() const { return fps; }
        float GetFrameTime() const { return frameTime; }
        int GetFrameCount() const { return frameCount; }

        static float GetRealtimeSinceStartup();
    };
}