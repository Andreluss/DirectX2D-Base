#pragma once
#include "common.h"
#include <unordered_map>

class App
{
public:
    struct EventInit {};
    struct EventUpdate {};
    struct EventDraw {};
    // Information about the time elapsed between frames.
    struct Time {
    private:
        const static std::chrono::steady_clock::time_point appStartTime;
    public:
        static float time;
        static float deltaTime;
        static float now() {
            auto time_now = std::chrono::high_resolution_clock::now();
            auto duration_from_start = time_now - appStartTime;
            
            return std::chrono::duration<float>(duration_from_start).count();
        }
    };
    struct Config {
        static float maxFPS;
        static float screen_width;
        static float screen_height;
    };
    static ComPtr<ID2D1Factory> GetFactory() {
        return d2d1_factory;
    }
    static ComPtr<ID2D1HwndRenderTarget> GetRenderTarget() {
        return d2d1_render_target;
    }
    static bool IsLoaded() { return isLoaded; }
    class Resources {
        static std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap>> bitmaps;
    public:
        static void Reset() { 
            bitmaps.clear(); 
        }
        static HRESULT GetBitmap(PCWSTR uri, ComPtr<ID2D1Bitmap>* ppBitmap) {
            auto it = bitmaps.find(uri);
            if (it != bitmaps.end()) {
                *ppBitmap = it->second;
                return S_OK;
            }
            else {
                auto hr = LoadBitmapFromFile(uri, 0, 0, ppBitmap);
                if (FAILED(hr)) {
                    return hr;
                }
                bitmaps[uri] = *ppBitmap;
                return hr;
            }
        }
    };
public:
    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

    static HRESULT LoadBitmapFromFile(PCWSTR uri,
        UINT destinationWidth,
        UINT destinationHeight,
        ComPtr<ID2D1Bitmap>* ppBitmap
    );

protected:
    // This method can be overridden to handle custom messages.
    // Returns true if the message was captured (and shouldn't be passed further).
    virtual bool CustomMessageHandler(
        [[maybe_unused]] UINT message,
        [[maybe_unused]] WPARAM wParam,
        [[maybe_unused]] LPARAM lParam
    ) {
        return false;
    }

    virtual void Update() = 0;
    virtual HRESULT InitApp() = 0;
    virtual void DropApp() = 0;


    // wic_factory:
    static IWICImagingFactory* wic_factory;


    struct Screen {
        static float width() {
            update();
            return static_cast<float>(rc.right - rc.left);
        }
        static float height() {
            update();
            return static_cast<float>(rc.bottom - rc.top);
        }
        static float centerX() {
            return width() / 2;
        }
        static float centerY() {
            return height() / 2;
        }
        static D2D1_SIZE_U d2d1_size_u() {
            update();
            D2D1_SIZE_U size = D2D1::SizeU(
                rc.right - rc.left,
                rc.bottom - rc.top
            );
            return size;
        }
        static HWND GetHwnd() { return App::hwnd_; }
    private:
        static RECT rc;
        static void update() {
            Assert(GetHwnd() != nullptr && "Querying for window info when it doesn't exist. ");
            if(GetHwnd()) GetClientRect(GetHwnd(), &rc);
        }
    };
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

    static HWND hwnd_;
    static ComPtr<ID2D1Factory> d2d1_factory;
    static ComPtr<ID2D1HwndRenderTarget> d2d1_render_target;
    static bool isLoaded;
};