#include "GrillApp.h"
#include <windowsx.h>

void GrillApp::Update()
{
    D2D1_SIZE_F rtSize = GetRenderTarget()->GetSize();

    // Draw a grid background.
    int width = static_cast<int>(rtSize.width);
    int height = static_cast<int>(rtSize.height);

    for (int x = 0; x < width; x += 10)
    {
        GetRenderTarget()->DrawLine(
            D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
            D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
            m_pLightSlateGrayBrush.Get(),
            0.5f
        );
    }

    for (int y = 0; y < height; y += 10)
    {
        GetRenderTarget()->DrawLine(
            D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
            D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
            m_pLightSlateGrayBrush.Get(),
            0.5f
        );
    }

    // Draw two rectangles.
    D2D1_RECT_F rectangle1 = D2D1::RectF(
        rtSize.width / 2 - 50.0f,
        rtSize.height / 2 - 50.0f,
        rtSize.width / 2 + 50.0f,
        rtSize.height / 2 + 50.0f
    );

    D2D1_RECT_F rectangle2 = D2D1::RectF(
        rtSize.width / 2 - 100.0f,
        rtSize.height / 2 - 100.0f,
        rtSize.width / 2 + 100.0f,
        rtSize.height / 2 + 100.0f
    );

    // Draw a filled rectangle.
    GetRenderTarget()->FillRectangle(&rectangle1, m_pLightSlateGrayBrush.Get());

    // Draw the outline of a rectangle.
    GetRenderTarget()->DrawRectangle(&rectangle2, m_pCornflowerBlueBrush.Get());

    for (int i = 0; i < 16; i++) {
        // meats on 4x4 grid with 10px padding (they are 100px wide), should be relative to center of screen
        m_meats[i]->transform.position =
            D2D1::Point2F(
                screen.centerX() + 110.0f * (i % 4 - 1.5f),
                screen.centerY() + 110.0f * (static_cast<float>(i / 4) - 1.5f)
            );
        m_meats[i]->Update(time.deltaTime);
    }
}

bool GrillApp::CustomMessageHandler(UINT message, WPARAM /*wParam*/, LPARAM lParam)
{
    if (message == WM_LBUTTONDOWN)
    {
        // Get the mouse position.
        D2D1_POINT_2F pt = {
            .x = static_cast<float>(GET_X_LPARAM(lParam)),
            .y = static_cast<float>(GET_Y_LPARAM(lParam)),
        };
        // Check to see if the mouse is inside the circle with center at 50, 60 and radius 40:

        for (auto& meat : m_meats) {
            if (!meat) continue;

            bool contains_point = pow(meat->transform.position.x - pt.x, 2) 
                                  + pow(meat->transform.position.y - pt.y, 2) 
                                  < pow(meat->radius, 2);

            if (contains_point) {
                meat->Collect();
                return true;
            }
        }
    }
    return false;
}

HRESULT GrillApp::CreateDeviceResourcesUser()
{
    // Create a gray brush.
    HRESULT hr = S_OK;

    hr = GetRenderTarget()->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::LightSlateGray),
        &m_pLightSlateGrayBrush
    );

    if (SUCCEEDED(hr))
    {
        // Create a blue brush.
        hr = GetRenderTarget()->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
            &m_pCornflowerBlueBrush
        );
    }

    if (SUCCEEDED(hr)) {
        // make it a matrix of 16 meats
        m_meats = std::vector<std::unique_ptr<Meat>>(16);
        for (int i = 0; i < 16; i++) {
            m_meats[i] = std::make_unique<Meat>(10.0f);
            hr = m_meats[i]->InitResources(GetRenderTarget());
            if (FAILED(hr)) {
                return hr;
            }
        }
    }
    return hr;
}

void GrillApp::DiscardDeviceResourcesUser()
{
    m_pLightSlateGrayBrush.Reset();
    m_pCornflowerBlueBrush.Reset();
}
