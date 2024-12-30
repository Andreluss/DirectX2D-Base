#pragma once
#include "common.h"

#include "core/Transform.h"
#include "core/Collider.h"
#include "core/GameObject.h"

using Microsoft::WRL::ComPtr;

class Meat : public GameObject
{
public: 
    using Idx = int;
    Idx GetIdx() const { return idx; }
private:
    float time_elapsed{};

    // Whether the meat is still on the grill (and can be collected).
    bool exists{ true };

    // These should be strictly increasing: 
    float time_to_cook;
    float time_to_burn;
    float time_max;

    Idx idx; 

    ComPtr<ID2D1HwndRenderTarget> render_target;
    // Brush to render the meat.
    ComPtr<ID2D1SolidColorBrush> m_pMeatBrush;

    void DrawProgressRing(float progress);
public:
    Meat(float total_time, Idx idx) : time_to_cook(total_time * 0.5f), time_to_burn(total_time * 0.75f), time_max(total_time), idx(idx) {}
    ~Meat() = default;

    // Collect the meat from the grill.
    void Collect();

    // ----------- common methods for game objects -----------
    HRESULT Init() override;
    void Update(float delta_time) override;

    // todo: add collider which has a reference to gameobject's transform
    // -------------------------------------------------------
    const float radius = 40.0f;
    // Event published when meat disappears from the grill. 
    struct Event {
        enum {
            CollectRaw,
            CollectCooked,
            CollectBurnt,
            NotCollected
        } type;
        Idx meat_idx;
    };
};

