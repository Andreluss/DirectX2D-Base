#pragma once
#include "core/GameObject.h"
#include "Meat.h"

class Grill : public GameObject
{
private:
    HRESULT InitResources() override;
    void Update() override;
    void Draw() override;
    void CustomMessageHandler(UINT message, WPARAM wParam, LPARAM lParam) override;

    ComPtr<ID2D1SolidColorBrush> grillBrush;
    ComPtr<ID2D1SolidColorBrush> m_pBlackBrush;
    
    int size_rt;
    std::vector<std::unique_ptr<Meat>> m_meats;

    EventSubscription<Meat::Event> meat_event_subscription{ [this](Meat::Event event) {
        // find and nullptr the meat with the given idx
        for (auto& meat : m_meats) {
            if (!meat) continue;
            if (meat->GetIdx() == event.meat_idx) {
                meat.reset();
                break;
            }
        }
    } };
public:
    // Creates a grill with size_rt x size_rt slots for meats.
    Grill(int size_rt) : size_rt(size_rt), m_meats(size_rt * size_rt)
    {

    }
    void SpawnMeat(float total_meat_time);
    void Clear();
    int MeatsCount() const;
};

