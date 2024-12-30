#pragma once
#include "common.h"

class App
{
public:
    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

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
        float time = now();
        float deltaTime {};
        float now() {
            return std::chrono::duration<float>(std::chrono::
                high_resolution_clock::now().time_since_epoch()).count();
        }
    } time;
    struct Config {
        float maxFPS = 60.0f;
    } config;
    struct Screen {
        HWND* m_hwnd_ptr;
        Screen(HWND* hwnd) : m_hwnd_ptr(hwnd) {}
        
        float width()  {
            update();
            return static_cast<float>(rc.right - rc.left);
        }
        float height() {
            update();
            return static_cast<float>(rc.bottom - rc.top);
        }
        float centerX() {
            return width() / 2;
        }
        float centerY() {
            return height() / 2;
        }
        D2D1_SIZE_U d2d1_size_u() {
            update();
            D2D1_SIZE_U size = D2D1::SizeU(
                rc.right - rc.left,
                rc.bottom - rc.top
            );
            return size;
        }
    private:
        void update() {
            ::GetClientRect(*m_hwnd_ptr, &rc);
        }
        RECT rc {};
    } screen = Screen(&m_hwnd);
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