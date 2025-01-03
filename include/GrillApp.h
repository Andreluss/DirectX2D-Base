#pragma once
#include "core/App.h"
#include "GrillGame.h"

class GrillApp : public App {
public:
    GrillApp() {
        App::Config::screen_height = 800;
        App::Config::screen_width = 1000;
        App::Config::window_class_name = L"GrillApp";
        App::Config::window_title = L"Grill Master";
    }
protected:
    HRESULT InitApp() override;
    void DropApp() override;
private:
    std::unique_ptr<GrillGame> game; // todo: change to std::unique_ptr<GameObject>
};