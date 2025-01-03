#pragma once
#include "core/App.h"
#include "GrillGame.h"

class GrillApp : public App {
protected:
    HRESULT InitApp() override;
    void DropApp() override;
    //void Update() override;
    //bool CustomMessageHandler(UINT message, WPARAM wParam, LPARAM lParam) override;
private:
    std::unique_ptr<GrillGame> game; // todo: change to std::unique_ptr<GameObject>
};