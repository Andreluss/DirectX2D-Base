#include "ProgressBar.h"

HRESULT ProgressBar::InitResources()
{
    HRESULT hr = S_OK;
    hr = App::GetRenderTarget()->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        &border_brush
    );
    if (SUCCEEDED(hr))
    {
        hr = App::GetRenderTarget()->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White),
            &fill_brush
        );
    }
    return hr;
}

void ProgressBar::Update()
{
    elapsed_time += Time::deltaTime;
}

void ProgressBar::Draw()
{
    float progress = min(1.0f, elapsed_time / duration);
    // Draw progress bar inside rect and outline
    D2D1_RECT_F rect = D2D1::RectF(
        transform.position.x,
        transform.position.y,
        transform.position.x + size.width,
        transform.position.y + size.height
    );
    App::GetRenderTarget()->FillRectangle(
        D2D1::RectF(
            rect.left + 2.0f,
            rect.top + 2.0f,
            rect.left + 2.0f + progress * (size.width - 4.0f),
            rect.bottom - 2.0f
        ),
        fill_brush.Get()
    );
    App::GetRenderTarget()->DrawRectangle(
        rect,
        border_brush.Get(),
        4.0f
    );
}
