#include "framework.h"
#include "EngineTime.h"

namespace AronEngine
{
    static std::chrono::steady_clock::time_point g_startupTime = std::chrono::steady_clock::now();

    Time::Time()
        : deltaTime(0.0f)
        , timeScale(1.0f)
        , totalTime(0.0f)
        , fps(0.0f)
        , frameTime(0.0f)
        , frameCount(0)
        , fpsUpdateTime(0.0f)
    {
    }

    void Time::Start()
    {
        startTime = std::chrono::steady_clock::now();
        lastFrameTime = startTime;
        currentFrameTime = startTime;
    }

    void Time::Update()
    {
        currentFrameTime = std::chrono::steady_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - lastFrameTime);
        deltaTime = duration.count() / 1000000.0f;
        
        frameTime = deltaTime;
        totalTime += deltaTime;
        frameCount++;

        fpsUpdateTime += deltaTime;
        if (fpsUpdateTime >= 1.0f)
        {
            fps = frameCount / fpsUpdateTime;
            frameCount = 0;
            fpsUpdateTime = 0.0f;
        }

        lastFrameTime = currentFrameTime;
    }

    float Time::GetRealtimeSinceStartup()
    {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - g_startupTime);
        return duration.count() / 1000000.0f;
    }
}