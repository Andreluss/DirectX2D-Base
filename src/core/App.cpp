// Source adapted from: https://learn.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart#part-2-implement-the-class-infrastructure
#include "core/App.h"
#include "core/EventSystem.h"

// Statics initialization
float App::Config::maxFPS{ 120.0f };
const std::chrono::steady_clock::time_point App::Time::appStartTime = std::chrono::high_resolution_clock::now();
float App::Time::time = App::Time::now();
float App::Time::deltaTime{};
ComPtr<ID2D1Factory> App::d2d1_factory{ nullptr };
ComPtr<ID2D1HwndRenderTarget> App::d2d1_render_target{ nullptr };
IWICImagingFactory* App::wic_factory{ nullptr };
bool App::isLoaded{ false };
HWND App::hwnd_{};
RECT App::Screen::rc{};
float App::Config::screen_height = 600;
float App::Config::screen_width = 800;
std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap>> App::Resources::bitmaps{};

void App::RunMessageLoop()
{
    MSG msg;

    do {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Trigger a redraw with FPS cap. 
        else if (Time::now() - Time::time >= 1.0 / Config::maxFPS) {
            InvalidateRect(hwnd_, NULL, TRUE);
        }
    } while (msg.message != WM_QUIT);
}

/// https://github.com/microsoft/Windows-classic-samples/blob/263dd514ad215d0a40d1ec44b4df84b30ec11dcf/Samples/Win7Samples/multimedia/Direct2D/SimpleDirect2DApplication/SimpleDirect2dApplication.cpp#L784
/// Example usage (Create a bitmap by loading it from a file:
/// 
/// hr = LoadBitmapFromFile(
///     d2d1_render_target,
///     m_pWICFactory,
///     L".\\sampleImage.jpg",
///     100,
///     0,
///     &m_pAnotherBitmap
///
HRESULT App::LoadBitmapFromFile(PCWSTR uri,
    UINT destinationWidth,
    UINT destinationHeight,
    ComPtr<ID2D1Bitmap>* ppBitmap
)
{
    HRESULT hr = S_OK;

    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    hr = wic_factory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
    );
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }

    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = wic_factory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = wic_factory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                        pSource,
                        destinationWidth,
                        destinationHeight,
                        WICBitmapInterpolationModeCubic
                    );
                }
                if (SUCCEEDED(hr))
                {
                    hr = pConverter->Initialize(
                        pScaler,
                        GUID_WICPixelFormat32bppPBGRA,
                        WICBitmapDitherTypeNone,
                        NULL,
                        0.f,
                        WICBitmapPaletteTypeMedianCut
                    );
                }
            }
        }
        else // Don't scale the image.
        {
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
            );
        }
    }
    if (SUCCEEDED(hr))
    {
        // Create a Direct2D bitmap from the WIC bitmap.
        ID2D1Bitmap* raw_pBitmap{ nullptr };
        hr = GetRenderTarget()->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            &raw_pBitmap
        );
        if (SUCCEEDED(hr)) {
            *ppBitmap = raw_pBitmap;
        }
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}


HRESULT App::Initialize()
{
    HRESULT hr;

    // Initialize device-independent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = App::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.lpszClassName = L"D2DApp";

        RegisterClassEx(&wcex);

        // In terms of using the correct DPI, to create a window at a specific size
        // like this, the procedure is to first create the window hidden. Then we get
        // the actual DPI from the HWND (which will be assigned by whichever monitor
        // the window is created on). Then we use SetWindowPos to resize it to the
        // correct DPI-scaled size, then we use ShowWindow to show it.

        hwnd_ = CreateWindow(
            L"D2DApp",
            L"Direct2D demo application",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this);

        if (hwnd_)
        {
            // Because the SetWindowPos function takes its size in pixels, we
            // obtain the window's DPI, and use it to scale the window size.
            float dpi = static_cast<float>(GetDpiForWindow(hwnd_));

            RECT r = { 0, 0, static_cast<LONG>(ceil(App::Config::screen_width * dpi / 96.f)), static_cast<LONG>(ceil(App::Config::screen_height * dpi / 96.f)) };
            AdjustWindowRect(&r, GetWindowLong(hwnd_, GWL_STYLE), FALSE);

            float window_width = static_cast<float>(r.right - r.left);
            float window_height = static_cast<float>(r.bottom - r.top);
            
            SetWindowPos(
                hwnd_,
                NULL,
                NULL,
                NULL,
                static_cast<int>(ceil(window_width * dpi / 96.f)),
                static_cast<int>(ceil(window_height * dpi / 96.f)),
                SWP_NOMOVE);
            ShowWindow(hwnd_, SW_SHOWNORMAL);
            UpdateWindow(hwnd_);
        }
    }

    return hr;
}

void App::Tick()
{
    event_system<EventUpdate>.Publish({});
    event_system<EventDraw>.Publish({});
}

HRESULT App::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2d1_factory.GetAddressOf());

    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wic_factory)
        );
    }

    return hr;
}

HRESULT App::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!d2d1_render_target)
    {
        RECT rc;
        GetClientRect(hwnd_, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = d2d1_factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hwnd_, size),
            &d2d1_render_target
        );

        if (SUCCEEDED(hr))
        {
            isLoaded = true;
            hr = InitApp();
            event_system<EventInit>.Publish(EventInit{});
        }
    }

    return hr;
}

void App::DiscardDeviceResources()
{
    isLoaded = false;
    d2d1_render_target.Reset();
    //Resources::Reset();
    DropApp();
}

HRESULT App::OnRender()
{
    HRESULT hr = S_OK;

    // Update frame time.
    float time_now = Time::now();
    Time::deltaTime = time_now - Time::time;
    Time::time = time_now;

    hr = CreateDeviceResources();
    if (SUCCEEDED(hr))
    {
        d2d1_render_target->BeginDraw();
        d2d1_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
        d2d1_render_target->Clear(D2D1::ColorF(D2D1::ColorF::White));

        Tick();

        hr = d2d1_render_target->EndDraw();
    }

    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }
    
    return hr;
}

void App::OnResize(UINT width, UINT height)
{
    if (d2d1_render_target)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        d2d1_render_target->Resize(D2D1::SizeU(width, height));
    }
}

LRESULT CALLBACK App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        App* pApp = (App*)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pApp)
        );

        result = 1;
    }
    else
    {
        App* pApp = reinterpret_cast<App*>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
            )));

        bool wasHandled = false;

        if (pApp)
        {
            if (pApp->GlobalMessageHandler(message, wParam, lParam)) {
                result = 0;
                wasHandled = true;
            }
            else {
                // (1/2) Handle the message for the app.
                switch (message)
                {
                case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pApp->OnResize(width, height);
                }
                result = 0;
                wasHandled = true;
                break;

                case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                result = 0;
                wasHandled = true;
                break;

                case WM_PAINT:
                {
                    pApp->OnRender();
                    ValidateRect(hwnd, NULL);
                }
                result = 0;
                wasHandled = true;
                break;

                case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                result = 1;
                wasHandled = true;
                break;
                }
                
                // (2/2) Publish the message to the event system.
                event_system<EventMessage>.Publish(EventMessage{ message, wParam, lParam });
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}