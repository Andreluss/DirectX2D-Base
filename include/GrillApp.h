#pragma once
#include "core/App.h"
#include "GrillGame.h"

class GrillApp : public App {
public:
    GrillApp() {
        App::Config::screen_height = 720;
        App::Config::screen_width = 1280;
    }
protected:
    HRESULT InitApp() override;
    void DropApp() override;
private:
    std::unique_ptr<GrillGame> game; // todo: change to std::unique_ptr<GameObject>
};