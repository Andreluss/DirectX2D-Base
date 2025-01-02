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
    else if (time_elapsed < time_to_disappear)
    {
        event.type = Meat::Event::CollectBurnt;
    }
    Assert(event.type != Meat::Event::NotCollected && L"Meat is completely burnt, yet collected.");

    event_system<Meat::Event>.Publish(event);
    exists = false;
}

HRESULT Meat::InitResources()
{
    // Create the brush.
    HRESULT hr = App::GetRenderTarget()->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::DarkRed),
        &meatBrush
    );

    if (SUCCEEDED(hr)) {
        hr = App::GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &progressBgBrush);
    }

    if (SUCCEEDED(hr)) {
        hr = App::Resources::GetBitmap(BitmapPaths::meat_raw, &meatRawBitmap);
    }

    if (SUCCEEDED(hr)) {
        hr = App::Resources::GetBitmap(BitmapPaths::meat_cooked, &meatCookedBitmap);
    }

    if (SUCCEEDED(hr)) {
        hr = App::Resources::GetBitmap(BitmapPaths::meat_burnt, &meatBurntBitmap);
    }

    if (SUCCEEDED(hr)) {
        hr = App::GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &progressBrush);
        progressBrush->SetOpacity(0.5f);
    }

    return hr;
}

static HRESULT DrawArc(
    float startAngle, float sweepAngle, float ring_radius, D2D1_POINT_2F center, 
    ComPtr<ID2D1SolidColorBrush> brush, float stroke_width) {
    // Convert angles to radians.
    float startAngleRad = startAngle * (3.14159f / 180.0f);
    float sweepAngleRad = sweepAngle * (3.14159f / 180.0f);

    // Calculate start and end points.
    D2D1_POINT_2F startPoint = D2D1::Point2F(
        center.x + ring_radius * cos(startAngleRad),
        center.y - ring_radius * sin(startAngleRad)
    );

    D2D1_POINT_2F endPoint = D2D1::Point2F(
        center.x + ring_radius * cos(startAngleRad + sweepAngleRad),
        center.y - ring_radius * sin(startAngleRad + sweepAngleRad)
    );

    HRESULT hr = S_OK;
    ComPtr<ID2D1PathGeometry> pathGeometry;
    hr = App::GetFactory()->CreatePathGeometry(&pathGeometry);
    if (SUCCEEDED(hr)) {
        // Create the path geometry sink.
        ComPtr<ID2D1GeometrySink> sink;
        hr = pathGeometry->Open(&sink);
        if (SUCCEEDED(hr)) {
            // Start the figure.

            // Draw the arc.
            D2D1_ARC_SEGMENT arcSegment{
                .point = endPoint,
                .size = D2D1::SizeF(ring_radius, ring_radius),
                .rotationAngle = 0.0f,
                .sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE,
                .arcSize = (std::abs(sweepAngle) > 180.0f) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL
            };

            // Begin drawing the figure.
            sink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN_FILLED /*D2D1_FIGURE_BEGIN_HOLLOW*/);
            sink->AddArc(arcSegment);
            sink->EndFigure(D2D1_FIGURE_END_OPEN);

            // Close the sink.
            hr = sink->Close();
        }

        if (SUCCEEDED(hr)) {
            // Draw the progress arc using the path geometry.
            App::GetRenderTarget()->DrawGeometry(pathGeometry.Get(), brush.Get(), stroke_width);
        }
    }
    return hr;
}

void Meat::DrawProgressRing(float progress) {
    // Clamp the progress to [0, 1].
    progress = max(0.0f, min(1.0f, progress));

    HRESULT hr = S_OK;
     // Define center and radius of the ring.
    D2D1_POINT_2F center = transform.position;
    float ring_radius = Meat::radius * 1.1f;

    // Draw the back segments of the progress ring. 
    if (SUCCEEDED(hr)) {
        progressBgBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
        progressBgBrush->SetOpacity(0.5f);
        if (SUCCEEDED(hr)) {
            // Draw the back segments of the progress ring.s
            hr = DrawArc(90.0f, -360.f * (time_to_cook / time_to_disappear), ring_radius, center, progressBgBrush, 8.0f);
        }
    }

    if (SUCCEEDED(hr)) {
        progressBgBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LawnGreen));
        progressBgBrush->SetOpacity(0.5f);
        if (SUCCEEDED(hr)) {
            // Draw the back segments of the progress ring.s
            hr = DrawArc(90.0f - 360.f * (time_to_cook / time_to_disappear),
                -360.f * ((time_to_burn - time_to_cook) / time_to_disappear),
                ring_radius, center, progressBgBrush, 8.0f);
        }
    }

    if (SUCCEEDED(hr)) {
        progressBgBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
        progressBgBrush->SetOpacity(0.5f);
        if (SUCCEEDED(hr)) {
            // Draw the back segments of the progress ring.s
            hr = DrawArc(90.0f - 360.f * (time_to_burn / time_to_disappear),
                -360.f * ((time_to_disappear - time_to_burn) / time_to_disappear),
                ring_radius, center, progressBgBrush, 8.0f);
        }
    }

    // Draw the main progress ring.
    if (SUCCEEDED(hr)) {
        // Define angles (in degrees).
        float startAngle = 90.0f;  // Starting at the top
        float sweepAngle = -360.f * progress; // Progress (e.g., 75% of a full circle)

        hr = DrawArc(startAngle, sweepAngle, ring_radius, center, progressBrush, 8.0f);
    }
}

void Meat::Update()
{
    float delta_time = Time::deltaTime;
    if (!exists) {
        return;
    }
    time_elapsed += delta_time;
}

void Meat::Draw()
{
    // Draw a circle representing the meat. The color should change depending on the time and the time_* parameters. 
    // First it is raw, then it is cooked, then it is burnt. At the end the meat disappears.

    ID2D1Bitmap* meatBitmap = meatBurntBitmap.Get();
    // Adjust the color of the meat.
    if (time_elapsed < time_to_cook)
    {
        // Raw meat.
        meatBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkRed));
        meatBitmap = meatRawBitmap.Get();
    }
    else if (time_elapsed < time_to_burn)
    {
        // Cooked meat.
        meatBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkOrange));
        meatBitmap = meatCookedBitmap.Get();
    }
    else if (time_elapsed < time_to_disappear)
    {
        // Burnt meat.
        meatBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkGray));
        meatBitmap = meatBurntBitmap.Get();
    }
    else
    {
        // Meat is gone.
        if (exists) {
            exists = false;
            event_system<Meat::Event>.Publish(
                Meat::Event{
                .type = Meat::Event::NotCollected,
                .meat_idx = idx
                }
            );
            // after this instruction the destructor has been called...
        }
        return;
    }

    DrawProgressRing(time_elapsed / time_to_disappear);

    //// Draw the meat circle.
    //meatBrush->SetOpacity(0.2f);
    //D2D1_ELLIPSE ellipse = D2D1::Ellipse(
    //    transform.position,
    //    radius,
    //    radius
    //);
    //App::GetRenderTarget()->FillEllipse(&ellipse, meatBrush.Get());

    // Draw the meat bitmap.
    App::GetRenderTarget()->DrawBitmap(
        meatBitmap,
        D2D1::RectF(
            transform.position.x - radius,
            transform.position.y - radius,
            transform.position.x + radius,
            transform.position.y + radius
        )
    );
}
