#pragma once
#include "App.h"
#include "Meat.h"

class GrillApp : public App {
public:
    GrillApp() = default;
protected:
    HRESULT CreateDeviceResourcesUser() override;
    void DiscardDeviceResourcesUser() override;
    void Update() override;
private:
    ComPtr<ID2D1SolidColorBrush> m_pLightSlateGrayBrush;
    ComPtr<ID2D1SolidColorBrush> m_pCornflowerBlueBrush;
    std::vector<std::unique_ptr<Meat>> m_meats;
};