#include <d2d1.h>
#include <iostream>
#include <Windows.h>

LRESULT CALLBACK WindowProc(
	HWND	hwnd,
	UINT	u_msg,
	WPARAM	w_param,
	LPARAM	l_param);

struct Position {
	int x = 0, y = 0;
};


int WINAPI wWinMain(
		HINSTANCE	h_instance,
		HINSTANCE	h_prev_instance,
		PWSTR		p_cmd_line,
		int			n_cmd_show) {

	const int kWinWidth = 500;
	const int kWinHeight = 309;
	const Position WinPosition;
	
	const wchar_t CLASS_NAME[] = L"2048 Class";

	WNDCLASS wc = {};
	wc.lpfnWndProc	 = WindowProc;
	wc.hInstance   	 = h_instance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		1,
		CLASS_NAME,
		L"2048",
		WS_OVERLAPPEDWINDOW,

		WinPosition.x, WinPosition.y, kWinWidth, kWinHeight,

		NULL,
		NULL,
		h_instance,
		NULL
	);

	if (hwnd == 0) {
		return 0;
	}

	ShowWindow(hwnd, n_cmd_show);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(
	HWND	hwnd,
	UINT	u_msg,
	WPARAM	w_param,
	LPARAM	l_param) {
	switch (u_msg) {
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		
		EndPaint(hwnd, &ps);
		return 0;
	}
	}
	return DefWindowProc(hwnd, u_msg, w_param, l_param);
}