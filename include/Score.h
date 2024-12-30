#pragma once
#include "core/GameObject.h"
#include "Meat.h"
#include "core/EventSystem.h"
#include <dwrite.h>

class Score : public GameObject
{
    int score{};
    int highscore{};

    ComPtr<IDWriteFactory> m_pDWriteFactory;
    ComPtr<IDWriteTextFormat> m_pTextFormat;
    ComPtr<ID2D1SolidColorBrush> m_pTextBrush;

protected:
    virtual HRESULT Init() override;

public:
    virtual void Update(float delta_time) override;
    void OnMeatEvent(Meat::Event event);
    int GetScore() { return score; }
    int getHighScore() { return highscore; }
    void Restart();

    // This bounds the subscription to the lifetime of the Score object.
    EventSubscription<Meat::Event> meat_event_subscription{ [this](Meat::Event event) {
        OnMeatEvent(event);
    } };
};

