#include "GrillApp.h"

HRESULT GrillApp::InitApp()
{
    // should throw if failed
    game = std::make_unique<GrillGame>();
    // currently, the hresult is not used
    return S_OK;
}

void GrillApp::DropApp()
{
    game.reset();
}
