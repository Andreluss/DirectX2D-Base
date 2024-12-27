#include "WinMain.h"

INT WINAPI wWinMain(
	_In_ [[maybe_unused]] HINSTANCE instance,
	_In_opt_ [[maybe_unused]] HINSTANCE prev_instance,
	_In_ [[maybe_unused]] PWSTR cmd_line,
	_In_ [[maybe_unused]] INT cmd_show
) {
	MessageBox(
		nullptr, TEXT("Cześć świecie!!!"),
		TEXT("Informacja"), MB_ICONINFORMATION
	);
	return 0;
}
