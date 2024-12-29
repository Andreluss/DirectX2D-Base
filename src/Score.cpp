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
            L"Verdana",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            24.0f,
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
            D2D1::ColorF(D2D1::ColorF::Red),
            &m_pTextBrush
        );
    }

    return hr;
}

void Score::Update(float /*delta_time*/)
{
    OutputDebugString(L"Score::Update\n");
    std::wstring score_str = std::wstring(L"Score: ") + std::to_wstring(score);
    render_target->DrawTextW(
        score_str.c_str(),
        9,
        m_pTextFormat.Get(),
        D2D1::RectF(transform.position.x, transform.position.y, 200, 40),
        m_pTextBrush.Get()
    );
}

void Score::OnMeatCollected(Meat::State state)
{
    switch (state)
    {
    case Meat::State::Cooked:
        score += 20;
        break;
    case Meat::State::Burnt:
    case Meat::State::Raw:
        score -= 5;
        break;
    case Meat::State::Gone:
        score -= 7;
        break;
    }
}

