#pragma once
#define WIN32_LEAN_AND_MEAN
#ifndef UNICODE
#define UNICODE
#endif 
#include "App.h"

INT WINAPI wWinMain(
    _In_ HINSTANCE instance,
    _In_opt_ HINSTANCE prev_instance,
    _In_ PWSTR cmd_line,
    _In_ INT cmd_show
);
