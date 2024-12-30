#include "Meat.h"
#include "core/EventSystem.h"
#include <stdexcept>
#include <numbers>

void Meat::Collect()
{
    if (!exists) {
        OutputDebugString(L"[WARN]: Meat doesn't exist and can't be collected.");
        return;
    }

    OutputDebugString(L"Meat collected.\n");
    Meat::Event event{
        .type = Meat::Event::NotCollected,
        .meat_idx = idx
    };
    if (time_elapsed < time_to_cook)
    {
        event.type = Meat::Event::CollectRaw;
    }
    else if (time_elapsed < time_to_burn)
    {
        event.type = Meat::Event::CollectCooked;
    }
    else if (time_elapsed < time_max)
    {
        event.type = Meat::Event::CollectBurnt;
    }
    Assert(event.type != Meat::Event::NotCollected && L"Meat is completely burnt, yet collected.");

    event_system<Meat::Event>.Publish(event);
    exists = false;
}

HRESULT Meat::Init()
{
    // Create the brush.
    HRESULT hr = render_target->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::DarkRed),
        &m_pMeatBrush
    );

    return hr;
}

void Meat::DrawProgressRing(float progress) {
    // Clamp the progress to [0, 1].
    progress = max(0.0f, min(1.0f, progress));

    // Draw the ring.
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        transform.position,
        radius,
        radius
    );

    // Draw the ring.
    render_target->DrawEllipse(&ellipse, m_pMeatBrush.Get(), 2.0f);
    render_target->FillEllipse(&ellipse, m_pMeatBrush.Get());

    // Draw the progress ring.
    D2D1_POINT_2F start_point = D2D1::Point2F(
        transform.position.x + 1.5f * radius * cosf(2 * std::numbers::pi_v<float> * progress),
        transform.position.y + 1.5f * radius * sinf(2 * std::numbers::pi_v<float> * progress)
    );
    render_target->DrawLine(transform.position, start_point, m_pMeatBrush.Get(), 2.0f);

    // Draw the sections of the ring, colored yellow, green and red depending on the progress.
    // use bezier curves to draw the circle segments 

    // https://docs.microsoft.com/en-us/windows/win32/direct2d/how-to-draw-bezier-curves

    auto draw_bezier = [&](D2D1_POINT_2F p0, D2D1_POINT_2F p1, D2D1_POINT_2F p2, D2D1_COLOR_F color) {
        
    };


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
        radius,
        radius
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
            exists = false;
            event_system<Meat::Event>.Publish(
                Meat::Event {
                .type = Meat::Event::NotCollected,
                .meat_idx = idx
                }
            );
            // after this instruction the destructor has been called...
        }
        return;
    }

    DrawProgressRing(time_elapsed / time_max);

    render_target->FillEllipse(&ellipse, m_pMeatBrush.Get());
}