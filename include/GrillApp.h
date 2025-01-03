#pragma once
#include "core/App.h"
#include "Meat.h"
#include "Score.h"
#include "ProgressBar.h"

class GrillApp : public App {
public:
    GrillApp() = default;
protected:
    HRESULT InitApp() override;
    void DropApp() override;
    void Update() override;
    bool CustomMessageHandler(UINT message, WPARAM wParam, LPARAM lParam) override;
private:
    ComPtr<ID2D1Bitmap> testBitmap;
    ComPtr<ID2D1SolidColorBrush> grillBrush;
    ComPtr<ID2D1SolidColorBrush> m_pBlackBrush;
    std::vector<std::unique_ptr<Meat>> m_meats;
    int MeatsCount() const {
        int count = 0;
        for (const auto& meat : m_meats) {
            if (meat) count++;
        }
        return count;
    }
    std::unique_ptr<Score> score;
    std::unique_ptr<ProgressBar> progress_bar;

    float gameProgress{};
    float nextMeatSpawnTime{};
    struct GameConfig {
        const int maxMeats = 16;
        const float gameDuration = 60.f;
        const float meatOnGrillTime = 6.f;
        const int finalMaxMeats = 9;
        const float timeToNextSpawn = 0.4f;
        std::vector<std::pair<float, int>> maxMeatsAtProgress = {
            {0.0f, 3},
            {0.05f, 4},
            {0.2f, 5},
            {0.3f, 6},
            {0.4f, 7},
            {0.6f, 8},
            {0.75f, finalMaxMeats}
        };
    } gameConfig;
    void UpdateGame(float delta_time);
    void SpawnMeat();

    void DrawGrill();

    EventSubscription<Meat::Event> meat_event_subscription{ [this](Meat::Event event) {
        // find and nullptr the meat with the given idx
        for (auto& meat : m_meats) {
            if (!meat) continue;
            if (meat->GetIdx() == event.meat_idx) {
                meat.reset();
                break;
            }
        }
    } };
};