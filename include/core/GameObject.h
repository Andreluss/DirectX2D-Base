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
    // Update state of the game, don't draw anything here.
    virtual void Update() = 0;
    // Draw the object on the screen.
    virtual void Draw() = 0;
    // [Optional] Handle custom messages (e.g. mouse click).
    virtual void CustomMessageHandler(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/) {}
    
    using Time = App::Time;
    using Screen = App::Screen;
    using Resources = App::Resources;

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
    EventSubscription<App::EventMessage> event_message_subscription{
        [this](App::EventMessage event) {
            CustomMessageHandler(event.message, event.wParam, event.lParam);
        }
    };
};

