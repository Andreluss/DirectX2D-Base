#pragma once
#include "core/GameObject.h"
#include "Meat.h"
#include <dwrite.h>

class Score : public GameObject
{
    int score{};

    ComPtr<IDWriteFactory> m_pDWriteFactory;
    ComPtr<IDWriteTextFormat> m_pTextFormat;
    ComPtr<ID2D1SolidColorBrush> m_pTextBrush;

protected:
    virtual HRESULT Init() override;

public:
    virtual void Update(float delta_time) override;
    void OnMeatCollected(Meat::State state);
};

