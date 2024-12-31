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
    ComPtr<ID2D1SolidColorBrush> textBgBrush;

protected:
    virtual HRESULT InitResources() override;

public:
    virtual void Update() override;
    void OnMeatEvent(Meat::Event event);
    int GetScore() { return score; }
    int getHighScore() { return highscore; }
    void Restart();

    Score()
    {
        InitResources();
    }

    // This bounds the subscription to the lifetime of the Score object.
    EventSubscription<Meat::Event> meat_event_subscription{ [this](Meat::Event event) {
        OnMeatEvent(event);
    } };
};

