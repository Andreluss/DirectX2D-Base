#pragma once
#include "core/Transform.h"

class GameObject
{
protected:
    virtual HRESULT Init() = 0;
public:
    ComPtr<ID2D1HwndRenderTarget> render_target;
    Transform transform {};
    virtual void Update(float delta_time) = 0;
    HRESULT InitGameObject(ComPtr<ID2D1HwndRenderTarget> _render_target) {
        render_target = _render_target;
        return Init();
    }
    //virtual void Drop() = 0;
};

