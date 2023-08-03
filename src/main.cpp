#include <d2d1.h>
#include <iostream>
#include <Windows.h>

#include "BaseWin.h"
#include "MainWindow.h"
#pragma comment(lib, "d2d1")

int WINAPI wWinMain(HINSTANCE h_instance, HINSTANCE, PWSTR, int n_cmd_show) {
	MainWindow win;

	if(!win.create(L"2048", WS_OVERLAPPEDWINDOW^WS_THICKFRAME, 0, 0, 0, 600, 600)) {
		return 0;
	}

	ShowWindow(win.window(), n_cmd_show);

	MSG msg = {};

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}