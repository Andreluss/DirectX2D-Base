#include "Grill.h"
#include <windowsx.h>


HRESULT Grill::InitResources()
{
    HRESULT hr = S_OK;

    hr = App::GetRenderTarget()->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::LightSlateGray),
        &grillBrush
    );

    if (SUCCEEDED(hr))
    {
        hr = App::GetRenderTarget()->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black),
            &m_pBlackBrush
        );
    }

    return hr;
}

void Grill::Update()
{
    for (int i = 0; i < ssize(m_meats); i++) {
        if (!m_meats[i]) continue;
        // meats on size_rt x size grid with, relative to center of screen
        m_meats[i]->transform.position = D2D1::Point2F(
            transform.position.x + transform.scale * 95.0f * (i % size_rt - (size_rt - 1.0f) / 2),
            transform.position.y + transform.scale * 95.0f * (static_cast<float>(i / size_rt) - (size_rt - 1.0f) / 2)
        );
        m_meats[i]->transform.scale = this->transform.scale;
    }
}

void Grill::Draw()
{
    float grill_grate_size = 400.0f;
    D2D1_RECT_F grill_grate_rect = D2D1::RectF(
        Screen::centerX() - grill_grate_size / 2,
        Screen::centerY() - grill_grate_size / 2,
        Screen::centerX() + grill_grate_size / 2,
        Screen::centerY() + grill_grate_size / 2
    );
    grillBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSlateGray));
    App::GetRenderTarget()->FillRectangle(&grill_grate_rect, grillBrush.Get());

    grillBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    for (auto x = grill_grate_rect.left; x < grill_grate_rect.right; x += 20)
    {
        App::GetRenderTarget()->DrawLine(
            D2D1::Point2F(x, grill_grate_rect.top),
            D2D1::Point2F(x, grill_grate_rect.bottom),
            grillBrush.Get(),
            1.f
        );
    }

    for (auto y = grill_grate_rect.top; y < grill_grate_rect.bottom; y += 20)
    {
        App::GetRenderTarget()->DrawLine(
            D2D1::Point2F(grill_grate_rect.left, y),
            D2D1::Point2F(grill_grate_rect.right, y),
            grillBrush.Get(),
            1.f
        );
    }

    float grill_border_size = grill_grate_size + 5.0f;
    D2D1_RECT_F grill_border_rect = D2D1::Rect(
        Screen::centerX() - grill_border_size / 2,
        Screen::centerY() - grill_border_size / 2,
        Screen::centerX() + grill_border_size / 2,
        Screen::centerY() + grill_border_size / 2
    );

    // rounded rect of grill border with 10px radius
    App::GetRenderTarget()->DrawRoundedRectangle(
        D2D1::RoundedRect(grill_border_rect, 10.0f, 10.0f),
        m_pBlackBrush.Get(),
        10.0f
    );
}

void Grill::CustomMessageHandler(UINT message, WPARAM /*wParam*/, LPARAM lParam)
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
                < pow(meat->radius * transform.scale, 2);

            if (contains_point) {
                meat->Collect();
                meat.reset();
                return;
            }
        }
    }
}


void Grill::SpawnMeat(float total_meat_time)
{
    Assert(total_meat_time > 0.05f && "Meat time is irreasonably <= 0.05s");

    // Where to spawn
    int spawn_idx = [&]() {
        std::vector<int> available_spawn_points;
        for (int i = 0; i < ssize(m_meats); i++) {
            if (!m_meats[i]) {
                available_spawn_points.push_back(i);
            }
        }
        Assert(!available_spawn_points.empty() && "No available spawn points");

        return available_spawn_points[rand() % available_spawn_points.size()];
        }();

    // Spawn 
    m_meats[spawn_idx] = std::make_unique<Meat>(total_meat_time, Meat::Idx(spawn_idx));
}

void Grill::Clear()
{
    for (auto& meat : m_meats) {
        meat.reset();
    }
}

int Grill::MeatsCount() const
{
    int count = 0;
    for (const auto& meat : m_meats) {
        if (meat) count++;
    }
    return count;
}
