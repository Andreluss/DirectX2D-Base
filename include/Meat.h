#pragma once
#include "common.h"

#include "core/Transform.h"
#include "core/Collider.h"
#include "core/GameObject.h"

class Meat : public GameObject
{
    struct BitmapPaths {
        static constexpr PCWSTR meat_raw = L".\\resources\\meat_raw.png";
        static constexpr PCWSTR meat_cooked = L".\\resources\\meat_cooked.png";
        static constexpr PCWSTR meat_burnt = L".\\resources\\meat_burnt.png";
        static constexpr PCWSTR meat_test = L".\\resources\\meat.png";
    };
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

    float radius = 40.0f;
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
    ComPtr<ID2D1SolidColorBrush> progressBgBrush;
    ComPtr<ID2D1SolidColorBrush> progressBrush;
    ComPtr<ID2D1SolidColorBrush> meatBrush;
    ComPtr<ID2D1Bitmap> meatRawBitmap{};
    ComPtr<ID2D1Bitmap> meatCookedBitmap{};
    ComPtr<ID2D1Bitmap> meatBurntBitmap{};

    void DrawProgressRing(float progress);

    HRESULT InitResources() override;
    void Update() override;
    void Draw() override;
};

