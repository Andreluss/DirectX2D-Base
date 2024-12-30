#include "Meat.h"
#include "core/EventSystem.h"
#include <stdexcept>

void Meat::Collect()
{
    if (!exists) {
        throw std::logic_error("Meat doesn't exist and can't be collected.");
    }

    OutputDebugString(L"Meat collected.\n");
    Meat::Event event; 
    {
        if (time_elapsed < time_to_cook)
        {
            event = Meat::Event::CollectRaw;
        }
        else if (time_elapsed < time_to_burn)
        {
            event = Meat::Event::CollectCooked;
        }
        else if (time_elapsed < time_max)
        {
            event = Meat::Event::CollectBurnt;
        }
        else
        {
            event = Meat::Event::NotCollected;
        }
    }

    event_system<Meat::Event>.Publish(event);
    exists = false;
}

HRESULT Meat::InitResources(ComPtr<ID2D1HwndRenderTarget> render_target)
{
    m_pRenderTarget = render_target;
    // Create the brush.
    HRESULT hr = render_target->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::DarkRed),
        &m_pMeatBrush
    );

    return hr;
}

void Meat::DropResources()
{
    m_pMeatBrush.Reset();
}

void Meat::Update(float delta_time)
{
    if (!exists) {
        return;
    }
    time_elapsed += delta_time;
    // Draw a circle representing the meat. The color should change depending on the time and the time_* parameters. 
    // First it is raw, then it is cooked, then it is burnt. At the end the meat disappears.

    // Draw the circle.
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        transform.position,
        50.0f,
        50.0f
    );
    // Adjust the color of the meat.
    if (time_elapsed < time_to_cook)
    {
        // Raw meat.
        m_pMeatBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkRed));
    }
    else if (time_elapsed < time_to_burn)
    {
        // Cooked meat.
        m_pMeatBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkOrange));
    }
    else if (time_elapsed < time_max)
    {
        // Burnt meat.
        m_pMeatBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkGray));
    }
    else
    {
        // Meat is gone.
        if (exists) {
            event_system<Meat::Event>.Publish(Meat::Event::NotCollected);
            exists = false;
        }
        return;
    }

    m_pRenderTarget->FillEllipse(&ellipse, m_pMeatBrush.Get());
}