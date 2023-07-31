#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Windows.h>
#include <d2d1.h>
#include "BaseWin.h"


template <typename Ty>
void safe_release(Ty **ppT) {
	if(*ppT) {
		(*ppT)->Release();
		(*ppT) = nullptr;
	}
}

class MainWindow : public Base_Win<MainWindow> {

public:
	MainWindow();

public:

	PCWSTR  class_name() const { return L"Circle Window Class"; }

	LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	ID2D1Factory* factory_{};
	ID2D1HwndRenderTarget* render_target_{};
	ID2D1SolidColorBrush* brush_{};
	D2D1_ELLIPSE ellipse_;
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
