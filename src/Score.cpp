#include "Score.h"
#include <xstring>

HRESULT Score::Init()
{
    OutputDebugString(L"Score::Init\n");

    HRESULT hr = S_OK;

    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(m_pDWriteFactory),
        &m_pDWriteFactory
    );

    if (SUCCEEDED(hr)) {
        hr = m_pDWriteFactory->CreateTextFormat(
            L"Open Sans",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            20.0f,
            L"en-us",
            &m_pTextFormat
        );
    }

    if (SUCCEEDED(hr)) {
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    }

    if (SUCCEEDED(hr)) {
        hr = render_target->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White),
            &m_pTextBrush
        );
    }

    if (SUCCEEDED(hr)) {
        hr = render_target->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black),
            &textBgBrush
        );
    }
    if (SUCCEEDED(hr)) {
        textBgBrush->SetOpacity(0.5f);
    }

    return hr;
}

void Score::Update(float /*delta_time*/)
{
    float bar_height = 75;
    // Draw the top UI bar opacity 0.8 black 
    auto size = render_target->GetSize();
    render_target->FillRectangle(
        D2D1::RectF(
            0, 0,
            size.width,
            bar_height
        ),
        textBgBrush.Get()
    );
    std::wstring score_str = std::wstring(L"Score: ") + std::to_wstring(score);
    score_str += L"\nHighscore: " + std::to_wstring(highscore);
    render_target->DrawText(
        score_str.c_str(),
        (UINT32)score_str.size(),
        m_pTextFormat.Get(),
        D2D1::RectF(transform.position.x, transform.position.y, 400, transform.position.y + bar_height - 10),
        m_pTextBrush.Get()
    );
}

void Score::OnMeatEvent(Meat::Event event)
{
    switch (event.type) {
    case Meat::Event::CollectRaw:
        score -= 2;
        break;
    case Meat::Event::CollectCooked:
        score += 5;
        break;
    case Meat::Event::CollectBurnt:
        score += 1;
        break;
    case Meat::Event::NotCollected:
        score -= 3;
        break;
    }
}

void Score::Restart()
{
    highscore = max(score, highscore);
    score = 0;
}

