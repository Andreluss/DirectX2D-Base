#pragma once
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <chrono>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

template<class Interface>
inline void SafeRelease(
    Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class App
{
public:
    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

protected:
    virtual void Update(); // float deltaTime?
    virtual HRESULT CreateDeviceResourcesUser() { 
        return S_OK;
    }; // todo-soon: pass getters here only?
    virtual void DiscardDeviceResourcesUser() {};
    ComPtr<ID2D1Factory> GetFactory() {
        return m_pDirect2dFactory;
    }
    ComPtr<ID2D1HwndRenderTarget> GetRenderTarget() {
        return m_pRenderTarget;
    }

    // Information about the time elapsed between frames.
    struct Time {
        float time {};
        float deltaTime {};
        float now() {
            return std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        }
    } time;
    struct Config {
        float maxFPS = 60.0f;
    } config;
private:

    // Initialize device-independent resources.
    HRESULT CreateDeviceIndependentResources();

    // Initialize device-dependent resources.
    HRESULT CreateDeviceResources();

    // Release device-dependent resource.
    void DiscardDeviceResources();

    // Draw content.
    HRESULT OnRender();

    // Resize the render target.
    void OnResize(
        UINT width,
        UINT height
    );

    // The windows procedure.
    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

private:
    HWND m_hwnd{};
    ComPtr<ID2D1Factory> m_pDirect2dFactory;
    ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
};