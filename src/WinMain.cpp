#include "WinMain.h"
#include <memory>

INT WINAPI wWinMain(
    _In_ [[maybe_unused]] HINSTANCE hInstance,
    _In_opt_ [[maybe_unused]] HINSTANCE prev_instance,
    _In_ [[maybe_unused]] PWSTR cmd_line,
    _In_ [[maybe_unused]] INT cmd_show
) {
    // Use HeapSetInformation to specify that the process should
   // terminate if the heap manager detects an error in any heap used
   // by the process.
   // The return value is ignored, because we want to continue running in the
   // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            auto app = std::make_unique<GrillApp>();
            if (auto hr = app->Initialize(); SUCCEEDED(hr)) 
            {
                app->RunMessageLoop();
            }
            else
            {
                return hr;
            }
        }
        CoUninitialize();
    }

    return 0;
}
