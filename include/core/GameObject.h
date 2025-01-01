#pragma once
#include "core/Transform.h"
#include "core/EventSystem.h"
#include "core/App.h"

/// Base for all objects that react to:
/// - InitResources() (reloads device-dependent resources)
/// - Update() (called every frame, uses App::Time::deltaTime to update logic)
/// - Draw() (called every frame after updates have finished to rerender object) 
/// The rule for initializing resources: 
/// - Device-dependent: InitResources() 
/// - Device-independent: constructor
class GameObject
{
public:
    Transform transform {};
private:
    virtual HRESULT InitResources() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    using Time = App::Time;

    EventSubscription<App::EventUpdate> event_update_subscription{
        [this](App::EventUpdate) {
            Update();
        }
    };
    EventSubscription<App::EventInit> event_init_subscription{
        [this](App::EventInit) {
            InitResources();
        }
    };
    EventSubscription<App::EventDraw> event_draw_subscription{
        [this](App::EventDraw) {
            Draw();
        }
    };
};

