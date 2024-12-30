#pragma once
#include "core/Transform.h"

class GameObject
{
protected:
    virtual HRESULT Init() = 0;
    ComPtr<ID2D1HwndRenderTarget> render_target;
    ComPtr<ID2D1Factory> d2d_factory;
public:
    Transform transform {};
    virtual void Update(float delta_time) = 0;
    HRESULT InitGameObject(ComPtr<ID2D1HwndRenderTarget> _render_target, ComPtr<ID2D1Factory> _factory) {
        render_target = _render_target;
        d2d_factory = _factory;
        return Init();
    }
    //virtual void Drop() = 0;
};

