#pragma once
#include "core/GameObject.h"
class ProgressBar : public GameObject
{
private:
    HRESULT InitResources() override;
    float duration;
    float elapsed_time{};
    ComPtr<ID2D1SolidColorBrush> border_brush;
    ComPtr<ID2D1SolidColorBrush> fill_brush;
public:
    D2D1_SIZE_F size;
    void Restart() { elapsed_time = 0.0f; }
    ProgressBar(float duration, D2D1_SIZE_F size) : duration(duration), size(size) {
        InitResources();
    }
    void Update() override;
    struct FinishedEvent {};
};

