#pragma once
#include "common.h"

#include "core/Transform.h"
#include "core/Collider.h"
#include "core/GameObject.h"

class Meat : public GameObject
{
public:
    using Idx = int;
    Meat(float total_time, Idx idx) : time_to_cook(total_time * 0.5f), time_to_burn(total_time * 0.7f), time_to_disappear(total_time), idx(idx) {
        InitResources();
    }
    ~Meat() = default;
    Meat(const Meat&) = default;

    Idx GetIdx() const { return idx; }

    // Collect the meat from the grill.
    void Collect();

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
private:
    float time_elapsed{};

    // Whether the meat is still on the grill (and can be collected).
    bool exists{ true };

    // These should be strictly increasing: 
    float time_to_cook;
    float time_to_burn;
    float time_to_disappear;

    Idx idx;

    // Brush to render the meat.
    ComPtr<ID2D1SolidColorBrush> meatBrush;

    void DrawProgressRing(float progress);

    HRESULT InitResources() override;
    void Update() override;
    void Draw() override;
};

