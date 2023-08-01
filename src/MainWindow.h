#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <string>
#include <array>

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <d2d1helper.h>

#include "BaseWin.h"


template <typename Ty>
void safe_release(Ty **ppT) {
	if(*ppT) {
		(*ppT)->Release();
		(*ppT) = nullptr;
	}
}

class MainWindow : public Base_Win<MainWindow> {

	const std::map<std::string, std::array<uint8_t, 3>> kColors = {
		{"2", {238, 228, 218}},
		{"4", {237, 224, 200}},
		{"8", {242, 177, 121}},
		{"16", {245, 149, 99}},
		{"32", {246, 124, 95}},
		{"64", {245, 98, 61}},
	};

public:
	MainWindow();

public:

	PCWSTR  class_name() const { return L"Circle Window Class"; }

	LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	ID2D1Factory* factory_{};
	ID2D1HwndRenderTarget* render_target_{};
	ID2D1SolidColorBrush* brush_{};
	IDWriteFactory* write_factory_{};
	IDWriteTextFormat* text_format_{};
	D2D1_RECT_F rect_;
	D2D1_POINT_2F mouse_{};

	void calculate_layout();

	void discard_graphics_resources();

	void on_paint();

	void resize();

	/*void on_l_button_down(int pixel_x, int pixel_y, DWORD flags);

	void on_l_button_up();

	void on_mouse_move(int pixel_x, int pixel_y, DWORD flags);*/

	HRESULT create_graphics_resources();

};



#endif
