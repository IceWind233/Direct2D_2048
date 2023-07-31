#include "MainWindow.h"

#include <Windows.h>
#pragma comment(lib, "d2d1")
MainWindow::MainWindow() :
	ellipse_(D2D1::Ellipse(D2D1_POINT_2F(), 0, 0)),
	mouse_(D2D1::Point2F()){ }

LRESULT MainWindow::handle_message(UINT u_msg, WPARAM w_param, LPARAM l_param) {
	switch (u_msg) {
	case WM_CREATE: {
		if(FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory_
		))) {
			return -1;
		}
		return 0;
	}

	case WM_DESTROY: {
		discard_graphics_resources();
		safe_release(&factory_);
		PostQuitMessage(0);
		return 0;
	}

	case WM_PAINT: {
		on_paint();
		return 0;
	}

	case WM_SIZE: {
		resize();
		return 0;
	}

	}
	return DefWindowProc(m_hwnd_, u_msg, w_param, l_param);
}

void MainWindow::calculate_layout() {
	if(render_target_ != nullptr) {
		D2D1_SIZE_F size = render_target_->GetSize();
		const float x = size.width / 2;
		const float y = size.height / 2;
		const float radius = min(x, y);
		ellipse_ = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}
}

void MainWindow::discard_graphics_resources() {
	safe_release(&render_target_);
	safe_release(&brush_);
}

void MainWindow::on_paint() {
	auto hr = create_graphics_resources();
	if(SUCCEEDED(hr)) {
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd_, &ps);

		render_target_->BeginDraw();
		render_target_->Clear(D2D1::ColorF(D2D1::ColorF::Azure));
		render_target_->FillEllipse(ellipse_, brush_);

		hr = render_target_->EndDraw();
		if(FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
			discard_graphics_resources();
		}
		EndPaint(m_hwnd_, &ps);
	}

}

void MainWindow::resize() {
	if(render_target_ != nullptr) {
		RECT rect;
		GetClientRect(m_hwnd_, &rect);

		D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
		render_target_->Resize(size);
		calculate_layout();
		InvalidateRect(m_hwnd_, nullptr, FALSE);
	}

}

/*void MainWindow::on_l_button_down(int pixel_x, int pixel_y, DWORD flags) {
}

void MainWindow::on_l_button_up() {
}

void MainWindow::on_mouse_move(int pixel_x, int pixel_y, DWORD flags) {
}*/

HRESULT MainWindow::create_graphics_resources() {
	HRESULT hr = S_OK;
	if(render_target_ == nullptr) {
		RECT rect;
		GetClientRect(m_hwnd_, &rect);

		D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
		hr = factory_->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd_, size),
			&render_target_);

		if(SUCCEEDED(hr)) {
			const D2D1_COLOR_F color = D2D1::ColorF(1.f, 1.f, 0);
			hr = render_target_->CreateSolidColorBrush(color, &brush_);
			if(SUCCEEDED(hr)) {
				calculate_layout();
			}
		}
	}
	return hr;
}
