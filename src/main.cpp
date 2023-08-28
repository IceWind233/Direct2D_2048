#include <d2d1.h>
#include <iostream>
#include <Windows.h>

#include "BaseWin.h"
#include "MainWindow.h"
#pragma comment(lib, "d2d1")

template <typename It>
auto func(It beg, It end) -> decltype(*beg + 0) {

	return *beg;
}

int WINAPI wWinMain(HINSTANCE h_instance, HINSTANCE, PWSTR, int n_cmd_show) {
	MainWindow win;

	if(!win.create(L"2048", WS_OVERLAPPEDWINDOW^WS_THICKFRAME, 0, 0, 0, 600, 600)) {
		return 0;
	}

	ShowWindow(win.window(), n_cmd_show);

	MSG msg = {};

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else {
			win.on_paint();
		}
	}

	return 0;
}