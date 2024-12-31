#pragma once
#include "core/Transform.h"
#include "core/EventSystem.h"
#include "core/App.h"

/// <summary>
/// Base for all objects that react to:
/// - InitResources() (reloads device-dependent resources)
/// - Update() (is called every frame with App::Time::deltaTime interval)
/// </summary>
class GameObject
{
public:
    Transform transform {};
private:
    virtual HRESULT InitResources() = 0;
    virtual void Update() = 0;
    using Time = App::Time;

    EventSubscription<App::EventUpdate> event_update_subscription{
        [this](App::EventUpdate) {
            Update();
        }
    };
    EventSubscription<App::EventInit> event_init_subscription{
        [this](App::EventInit)->void {
            this->InitResources();
        }
    };
};

