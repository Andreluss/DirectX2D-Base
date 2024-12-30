#pragma once
#include <windows.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

#include "core/Transform.h"
#include "core/Collider.h"

using Microsoft::WRL::ComPtr;

class Meat
{
private:
    float time_elapsed{};

    // Whether the meat is still on the grill (and can be collected).
    bool exists{ true };

    // These should be strictly increasing: 
    float time_to_cook;
    float time_to_burn;
    float time_max;

    ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
    // Brush to render the meat.
    ComPtr<ID2D1SolidColorBrush> m_pMeatBrush;
public:
    Meat(float total_time) : time_to_cook(total_time * 0.5f), time_to_burn(total_time * 0.75f), time_max(total_time) {}
    ~Meat() = default;

    // Collect the meat from the grill.
    void Collect();

    // ----------- common methods for game objects -----------
    HRESULT InitResources(ComPtr<ID2D1HwndRenderTarget> render_target);
    void DropResources();
    void Update(float delta_time);

    Transform transform {};
    // todo: add collider which has a reference to gameobject's transform
    // -------------------------------------------------------
    const float radius = 40.0f;
    // Event published when meat disappears from the grill. 
    enum class Event {
        CollectRaw,
        CollectCooked,
        CollectBurnt,
        NotCollected
    };
};

