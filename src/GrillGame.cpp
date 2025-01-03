#include "GrillGame.h"
#include "core/App.h"
#include <random>
#include <xstring>

// https://stackoverflow.com/a/36527160/15597349
static float random_float(float min, float max)
{
    static std::default_random_engine e;
    std::uniform_real_distribution<> dis(min, max); // range [0, 1)
    return static_cast<float>(dis(e));
}

void GrillGame::UpdateGame(float delta_time)
{
    if (gameProgress < 1.0f && gameProgress + delta_time / config.gameDuration >= 1.0f) {
        // Game finished! 
        // Show message and ask if want to play again, otherwise close the app'
        auto msg = (std::wstring(L"Game finished!\nYour score is ") + std::to_wstring(score->GetScore())) + L"\nPress OK to play again.";
        gameProgress += delta_time / config.gameDuration;
        if (MessageBox(Screen::GetHwnd(), msg.c_str(), L"Game Over", MB_RETRYCANCEL | MB_ICONINFORMATION) == IDRETRY) {
            // Restart the game
            gameProgress = 0.0f;
            nextMeatSpawnTime = Time::time;
            score->Restart();
            progress_bar->Restart();
            grill->Clear();
            Time::time = Time::now();
        }
        else {
            PostQuitMessage(0);
        }
    }
    gameProgress += delta_time / config.gameDuration;
    if (gameProgress > 1.0f) {
        gameProgress = 1.0f;
    }
    int max_meats = 1;
    for (auto& [progress, max] : config.maxMeatsAtProgress) {
        if (gameProgress < progress) {
            break;
        }
        max_meats = max(max, max_meats);
    }

    if (grill->MeatsCount() < max_meats && Time::time > nextMeatSpawnTime) {
        float meat_time = config.meatOnGrillTime * random_float(0.4f, 1.f);
        grill->SpawnMeat(meat_time);
        nextMeatSpawnTime = Time::time + config.timeToNextSpawn;
    }
}

HRESULT GrillGame::InitResources()
{
    // todo-someday: change error handling model 
    //  to exceptions thrown in constructors instead of HRESULT
    HRESULT hr = S_OK;

    // e.g. currently, errors from this line are ignored
    grill = std::make_unique<Grill>(config.grillSizeRt);

    if (SUCCEEDED(hr)) {
        score = std::make_unique<Score>();
        //hr = score->InitGameObject(GetRenderTarget(), GetFactory());
    }

    if (SUCCEEDED(hr)) {
        progress_bar = std::make_unique<ProgressBar>(config.gameDuration, D2D1::SizeF(400, 20));
        //hr = progress_bar->InitGameObject(GetRenderTarget(), GetFactory());
    }

    if (SUCCEEDED(hr)) {
        // todo: change to Resources get bitmap
        hr = App::LoadBitmapFromFile(
            L".\\resources\\background.jpg",
            (int)Screen::width(),
            0,
            &bgBitmap);
    }

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to load game resources, hr: " + std::to_string(hr));
    }

    return hr;
}

void GrillGame::Update()
{
    UpdateGame(Time::deltaTime);

    float scaling_ratio = min(Screen::width() / 800, Screen::height() / 600);
    grill->transform.scale = 1.f * scaling_ratio;
    grill->transform.position = D2D1::Point2F(Screen::centerX(), Screen::centerY());

    // grill->Update(); should happen now

    score->transform.position = D2D1::Point2F(10.0f, 10.0f);

    if (progress_bar) {
        progress_bar->transform.position = D2D1::Point2F(
            Screen::centerX() - progress_bar->size.width / 2,
            Screen::height() - progress_bar->size.height - 30.0f
        );
    }
}

void GrillGame::Draw() {
    // Draw background
    D2D1_SIZE_F size = bgBitmap->GetSize();
    float r = max(Screen::width() / size.width,
                  Screen::height() / size.height);

    App::GetRenderTarget()->DrawBitmap(
        bgBitmap.Get(),
        D2D1::RectF(
            Screen::centerX() - r * size.width / 2,
            Screen::centerY() - r * size.height / 2,
            Screen::centerX() + r * size.width / 2,
            Screen::centerY() + r * size.height / 2
        )
    );

    // grill->Draw(); this should happen now
}

