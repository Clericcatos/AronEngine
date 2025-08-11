#include <windows.h>
#include "Core/EngineApp.h"
#include "TestApp.h"

using namespace AronEngine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    CoInitialize(nullptr);

    TestApp app;
    
    if (app.Initialize(hInstance, 1600, 900, L"AronEngine - Game Editor", true))
    {
        app.Run();
    }

    app.Shutdown();
    CoUninitialize();

    return 0;
}