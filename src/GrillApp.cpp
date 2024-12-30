#include "GrillApp.h"
#include <windowsx.h>

void GrillApp::Update()
{
    D2D1_SIZE_F rtSize = GetRenderTarget()->GetSize();

    // Draw a dark green background.
    GetRenderTarget()->Clear(D2D1::ColorF(0x225522));

    // set brush color to light slate gray
    m_pLightSlateGrayBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSlateGray));

    // Draw two rectangles.
    float grill_grate_size = 380.0f;
    D2D1_RECT_F grill_grate_rect = D2D1::RectF(
        screen.centerX() - grill_grate_size / 2,
        screen.centerY() - grill_grate_size / 2,
        screen.centerX() + grill_grate_size / 2,
        screen.centerY() + grill_grate_size / 2
    );
    // Draw a filled rectangle.
    GetRenderTarget()->FillRectangle(&grill_grate_rect, m_pLightSlateGrayBrush.Get());

    // set color of brush to black
    m_pLightSlateGrayBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

    for (auto x = grill_grate_rect.left; x < grill_grate_rect.right; x += 20)
    {
        GetRenderTarget()->DrawLine(
            D2D1::Point2F(x, grill_grate_rect.top),
            D2D1::Point2F(x, grill_grate_rect.bottom),
            m_pLightSlateGrayBrush.Get(),
            0.5f
        );
    }

    for (auto y = grill_grate_rect.top; y < grill_grate_rect.bottom; y += 20)
    {
        GetRenderTarget()->DrawLine(
            D2D1::Point2F(grill_grate_rect.left, y),
            D2D1::Point2F(grill_grate_rect.right, y),
            m_pLightSlateGrayBrush.Get(),
            0.5f
        );
    }

    float grill_border_size = grill_grate_size + 5.0f;
    D2D1_RECT_F grill_border_rect = D2D1::Rect(
        rtSize.width / 2 - grill_border_size / 2,
        rtSize.height / 2 - grill_border_size / 2,
        rtSize.width / 2 + grill_border_size / 2,
        rtSize.height / 2 + grill_border_size / 2
    );
    // rounded rect of grill border with 10px radius
    GetRenderTarget()->DrawRoundedRectangle(
        D2D1::RoundedRect(grill_border_rect, 10.0f, 10.0f),
        m_pBlackBrush.Get(),
        10.0f
    );

    for (int i = 0; i < 16; i++) {
        if (!m_meats[i]) continue;
        // meats on 4x4 grid with 10px padding (they are 100px wide), should be relative to center of screen
        m_meats[i]->transform.position =
            D2D1::Point2F(
                screen.centerX() + 90.0f * (i % 4 - 1.5f),
                screen.centerY() + 90.0f * (static_cast<float>(i / 4) - 1.5f)
            );
        m_meats[i]->Update(time.deltaTime);
    }

    score->transform.position = D2D1::Point2F(10.0f, 10.0f);
    score->Update(time.deltaTime);
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
                meat.reset();
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
            D2D1::ColorF(D2D1::ColorF::Black),
            &m_pBlackBrush
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

    if (SUCCEEDED(hr)) {
        score = std::make_unique<Score>();
        hr = score->InitGameObject(GetRenderTarget());
    }

    return hr;
}

void GrillApp::DiscardDeviceResourcesUser()
{
    m_pLightSlateGrayBrush.Reset();
    m_pBlackBrush.Reset();
    for (auto& meat : m_meats) {
        if (!meat) continue;
        meat->DropResources();
    }
}
