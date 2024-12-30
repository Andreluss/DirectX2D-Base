#pragma once
#include "core/GameObject.h"
#include "Meat.h"
#include "core/EventSystem.h"
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
    void OnMeatEvent(Meat::Event event);
    Score() {
        //event_system<Meat::Event>.Subscribe(std::bind(&Score::OnMeatEvent, this, std::placeholders::_1));
        // use lambda
        event_system<Meat::Event>.Subscribe((void*)this, [this](Meat::Event event) {
            OnMeatEvent(event);
        });
    }
    ~Score() {
        event_system<Meat::Event>.Unsubscribe( (void*)this);
    }
};

