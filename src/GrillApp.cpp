#include "GrillApp.h"
#include <windowsx.h>
#include <random>

// https://stackoverflow.com/a/36527160/15597349
static float random_float(float min, float max)
{
    static std::default_random_engine e;
    std::uniform_real_distribution<> dis(min, max); // range [0, 1)
    return static_cast<float>(dis(e));
}

void GrillApp::UpdateGame(float delta_time)
{
    gameProgress += delta_time / gameConfig.gameDuration;
    if (gameProgress > 1.0f) {
        gameProgress = 1.0f;
    }
    int max_meats = 1;
    for (auto& [progress, max] : gameConfig.maxMeatsAtProgress) {
        if (gameProgress < progress) {
            break;
        }
        max_meats = max;
    }

    if (MeatsCount() < max_meats && time.time > nextMeatSpawnTime) {
        SpawnMeat();
    }
}

void GrillApp::SpawnMeat()
{
    Assert(nextMeatSpawnTime <= time.time && "Spawned meat too early");
    nextMeatSpawnTime = time.time + gameConfig.timeToNextSpawn;

    // Where to spawn
    int spawn_idx = [&]() {
        std::vector<int> available_spawn_points;
        for (int i = 0; i < gameConfig.maxMeats; i++) {
            if (!m_meats[i]) {
                available_spawn_points.push_back(i);
            }
        }
        Assert(!available_spawn_points.empty() && "No available spawn points");

        return available_spawn_points[rand() % available_spawn_points.size()];
    }();


    // What meat to spawn
    float total_meat_time = gameConfig.meatOnGrillTime 
        * random_float(0.3f, 1.f);
    Assert(total_meat_time > 0.05f && "Meat time is irreasonably <= 0.05s");

    // Spawn and initialize 
    m_meats[spawn_idx] = std::make_unique<Meat>(total_meat_time, Meat::Idx(spawn_idx));
    m_meats[spawn_idx]->Init(); // todo: run automatically when creating GameObject
}

void GrillApp::DrawGrill() {
    float grill_grate_size = 380.0f;
    D2D1_RECT_F grill_grate_rect = D2D1::RectF(
        screen.centerX() - grill_grate_size / 2,
        screen.centerY() - grill_grate_size / 2,
        screen.centerX() + grill_grate_size / 2,
        screen.centerY() + grill_grate_size / 2
    );
    m_pLightSlateGrayBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSlateGray));
    GetRenderTarget()->FillRectangle(&grill_grate_rect, m_pLightSlateGrayBrush.Get());

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
        screen.centerX() - grill_border_size / 2,
        screen.centerY() - grill_border_size / 2,
        screen.centerX() + grill_border_size / 2,
        screen.centerY() + grill_border_size / 2
    );

    // rounded rect of grill border with 10px radius
    GetRenderTarget()->DrawRoundedRectangle(
        D2D1::RoundedRect(grill_border_rect, 10.0f, 10.0f),
        m_pBlackBrush.Get(),
        10.0f
    );
}

void GrillApp::Update()
{
    // Draw a dark green background.
    GetRenderTarget()->Clear(D2D1::ColorF(0x225522));

    DrawGrill();

    for (int i = 0; i < 16; i++) {
        if (!m_meats[i]) continue;
        // meats on 4x4 grid with, relative to center of screen
        m_meats[i]->transform.position =
            D2D1::Point2F(
                screen.centerX() + 90.0f * (i % 4 - 1.5f),
                screen.centerY() + 90.0f * (static_cast<float>(i / 4) - 1.5f)
            );
        m_meats[i]->Update(time.deltaTime);
    }

    score->transform.position = D2D1::Point2F(10.0f, 10.0f);
    score->Update(time.deltaTime);

    UpdateGame(time.deltaTime);
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
        m_meats = std::vector<std::unique_ptr<Meat>>(gameConfig.maxMeats);
    }

    if (SUCCEEDED(hr)) {
        score = std::make_unique<Score>();
        hr = score->InitGameObject(GetRenderTarget(), GetFactory());
    }

    return hr;
}

void GrillApp::DiscardDeviceResourcesUser()
{
    m_pLightSlateGrayBrush.Reset();
    m_pBlackBrush.Reset();
    for (auto& meat : m_meats) {
        if (!meat) continue;
        meat.reset();
    }
}
