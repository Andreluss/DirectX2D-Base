#include "GrillApp.h"

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
    return hr;
}

void GrillApp::DiscardDeviceResourcesUser()
{
    m_pLightSlateGrayBrush.Reset();
    m_pCornflowerBlueBrush.Reset();
}
