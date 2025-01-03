#pragma once
#include "core/GameObject.h"
#include "Score.h"
#include "Grill.h"
#include "ProgressBar.h"

class GrillGame: public GameObject
{
private:
    HRESULT InitResources() override;
    void Update() override;
    void Draw() override;

    ComPtr<ID2D1Bitmap> bgBitmap;
    std::unique_ptr<Grill> grill;
    std::unique_ptr<Score> score;
    std::unique_ptr<ProgressBar> progress_bar;

    float gameProgress{};
    float nextMeatSpawnTime{};
    struct Config {
        const int grillSizeRt = 4;
        const float gameDuration = 60.f;
        const float meatOnGrillTime = 6.f;
        const int finalMaxMeats = 9;
        const float timeToNextSpawn = 0.4f;
        const std::vector<std::pair<float, int>> maxMeatsAtProgress = {
            {0.0f, 3},
            {0.05f, 4},
            {0.2f, 5},
            {0.3f, 6},
            {0.4f, 7},
            {0.6f, 8},
            {0.75f, finalMaxMeats}
        };
    } config;
    void UpdateGame(float delta_time);
};

