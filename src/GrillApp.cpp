#include "GrillApp.h"
#include <windowsx.h>
#include <random>
#include <xstring>

// https://stackoverflow.com/a/36527160/15597349
static float random_float(float min, float max)
{
    static std::default_random_engine e;
    std::uniform_real_distribution<> dis(min, max); // range [0, 1)
    return static_cast<float>(dis(e));
}

void GrillApp::UpdateGame(float delta_time)
{
    if (gameProgress < 1.0f && gameProgress + delta_time / gameConfig.gameDuration >= 1.0f) {
        // Game finished! 
        // Show message and ask if want to play again, otherwise close the app'
        auto msg = (std::wstring(L"Game finished!\nYour score is ") + std::to_wstring(score->GetScore())) + L"\nPress OK to play again.";
        if (MessageBox(Screen::GetHwnd(), msg.c_str(), L"Game Over", MB_RETRYCANCEL | MB_ICONINFORMATION) == IDRETRY) {
            // Restart the game
            gameProgress = 0.0f;
            nextMeatSpawnTime = Time::time;
            score->Restart();
            progress_bar->Restart();
            for (auto& meat : m_meats) {
                meat.reset();
            }
            Time::time = Time::now();
        }
        else {
            PostQuitMessage(0);
        }
    }
    gameProgress += delta_time / gameConfig.gameDuration;
    if (gameProgress > 1.0f) {
        gameProgress = 1.0f;
    }
    int max_meats = 1;
    for (auto& [progress, max] : gameConfig.maxMeatsAtProgress) {
        if (gameProgress < progress) {
            break;
        }
        max_meats = max(max, max_meats);
    }

    if (MeatsCount() < max_meats && Time::time > nextMeatSpawnTime) {
        SpawnMeat();
    }
}

void GrillApp::SpawnMeat()
{
    Assert(nextMeatSpawnTime <= Time::time && "Spawned meat too early");
    nextMeatSpawnTime = Time::time + gameConfig.timeToNextSpawn;

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
        * random_float(0.25f, 1.f);
    Assert(total_meat_time > 0.05f && "Meat time is irreasonably <= 0.05s");

    // Spawn and initialize 
    m_meats[spawn_idx] = std::make_unique<Meat>(total_meat_time, Meat::Idx(spawn_idx));
    //m_meats[spawn_idx]->InitGameObject(GetRenderTarget(), GetFactory()); // todo: run automatically when creating GameObject
}

void GrillApp::DrawGrill() {
    float grill_grate_size = 380.0f;
    D2D1_RECT_F grill_grate_rect = D2D1::RectF(
        Screen::centerX() - grill_grate_size / 2,
        Screen::centerY() - grill_grate_size / 2,
        Screen::centerX() + grill_grate_size / 2,
        Screen::centerY() + grill_grate_size / 2
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
        Screen::centerX() - grill_border_size / 2,
        Screen::centerY() - grill_border_size / 2,
        Screen::centerX() + grill_border_size / 2,
        Screen::centerY() + grill_border_size / 2
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

    D2D1_SIZE_F size = testBitmap->GetSize();
    auto renderTargetSize = GetRenderTarget()->GetSize();
    GetRenderTarget()->DrawBitmap(
        testBitmap.Get(),
        D2D1::RectF(
            renderTargetSize.width - size.width,
            renderTargetSize.height - size.height,
            renderTargetSize.width,
            renderTargetSize.height)
    );

    DrawGrill();

    for (int i = 0; i < 16; i++) {
        if (!m_meats[i]) continue;
        // meats on 4x4 grid with, relative to center of screen
        m_meats[i]->transform.position =
            D2D1::Point2F(
                Screen::centerX() + 90.0f * (i % 4 - 1.5f),
                Screen::centerY() + 90.0f * (static_cast<float>(i / 4) - 1.5f)
            );
        m_meats[i]->Update();
    }

    score->transform.position = D2D1::Point2F(10.0f, 10.0f);
    score->Update();

    if (progress_bar) {
        progress_bar->Update();
        progress_bar->transform.position = D2D1::Point2F(
            Screen::centerX() - progress_bar->size.width / 2,
            Screen::height() - progress_bar->size.height - 30.0f
        );
    }

    UpdateGame(Time::deltaTime);
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

HRESULT GrillApp::InitApp()
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
        //hr = score->InitGameObject(GetRenderTarget(), GetFactory());
    }

    if (SUCCEEDED(hr)) {
        progress_bar = std::make_unique<ProgressBar>(gameConfig.gameDuration, D2D1::SizeF(400, 20));
        //hr = progress_bar->InitGameObject(GetRenderTarget(), GetFactory());
    }

    if (SUCCEEDED(hr)) {
        hr = LoadBitmapFromFile(
            GetRenderTarget().Get(),
            wic_factory.Get(),
            L".\\sampleImage.jpg",
            (int)Screen::width(),
            0,
            &testBitmap
        );
    }

    return hr;
}

void GrillApp::DropApp()
{
    m_pLightSlateGrayBrush.Reset();
    m_pBlackBrush.Reset();
    for (auto& meat : m_meats) {
        if (!meat) continue;
        meat.reset();
    }
}
