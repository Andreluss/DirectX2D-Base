#pragma once
#include "App.h"
#include "Meat.h"
#include "Score.h"

class GrillApp : public App {
public:
    GrillApp() = default;
protected:
    HRESULT CreateDeviceResourcesUser() override;
    void DiscardDeviceResourcesUser() override;
    void Update() override;
    bool CustomMessageHandler(UINT message, WPARAM wParam, LPARAM lParam) override;
private:
    ComPtr<ID2D1SolidColorBrush> m_pLightSlateGrayBrush;
    ComPtr<ID2D1SolidColorBrush> m_pBlackBrush;
    std::vector<std::unique_ptr<Meat>> m_meats;
    std::unique_ptr<Score> score;
};