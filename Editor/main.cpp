#include <windows.h>
#include "Core/EngineApp.h"
#include "EditorApp.h"
#include <iostream>
#include <fstream>

using namespace AronEngine;

void WriteDebugLog(const std::string& message)
{
    std::ofstream log("editor_debug.log", std::ios::app);
    log << message << std::endl;
    log.close();
    OutputDebugStringA((message + "\n").c_str());
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WriteDebugLog("[Editor] Starting AronEditor...");
    
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        WriteDebugLog("[Editor] Failed to initialize COM: " + std::to_string(hr));
        MessageBoxA(nullptr, "Failed to initialize COM", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    WriteDebugLog("[Editor] COM initialized successfully");

    EditorApp app;
    
    WriteDebugLog("[Editor] Initializing EditorApp...");
    if (app.Initialize(hInstance, 1600, 900, L"AronEngine - Editor", true))
    {
        WriteDebugLog("[Editor] EditorApp initialized successfully, starting main loop");
        app.Run();
    }
    else
    {
        WriteDebugLog("[Editor] Failed to initialize EditorApp");
        MessageBoxA(nullptr, "Failed to initialize Editor", "Error", MB_OK | MB_ICONERROR);
    }

    WriteDebugLog("[Editor] Shutting down...");
    app.Shutdown();
    CoUninitialize();
    WriteDebugLog("[Editor] Shutdown complete");

    return 0;
}