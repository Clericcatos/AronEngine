#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <chrono>
#include <cmath>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <queue>
#include <stack>
#include <set>

#include <windows.h>
#include <wrl.h>
#include <d2d1_3.h>
#include <d2d1_3helper.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <d3d11_1.h>
#include <dxgi1_4.h>
#include <d2d1effects_2.h>


#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#ifdef _DEBUG
#define DEBUG_LOG(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
#define DEBUG_LOG(msg)
#endif

#include "../Utils/Vector2.h"
#include "../Utils/Color.h"

namespace AronEngine
{
    using namespace Microsoft::WRL;

    template<typename T>
    inline void SafeRelease(T** ppT)
    {
        if (*ppT)
        {
            (*ppT)->Release();
            *ppT = nullptr;
        }
    }

    template<typename T>
    inline void SafeDelete(T** ppT)
    {
        if (*ppT)
        {
            delete *ppT;
            *ppT = nullptr;
        }
    }

    template<typename T>
    inline void SafeDeleteArray(T** ppT)
    {
        if (*ppT)
        {
            delete[] *ppT;
            *ppT = nullptr;
        }
    }
}