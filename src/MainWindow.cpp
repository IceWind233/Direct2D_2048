#include "MainWindow.h"

#include <Windows.h>
#include <dwrite.h>
#include <d2d1helper.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
MainWindow::MainWindow() :
	rect_(D2D1::Rect(0, 0, 0, 0)),
	mouse_(D2D1::Point2F()) { }

LRESULT MainWindow::handle_message(UINT u_msg, WPARAM w_param, LPARAM l_param) {
	switch (u_msg) {
	case WM_CREATE: {
		if(FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory_
		))) {
			return -1;
		}
		DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&write_factory_));

		write_factory_->CreateTextFormat(
			L"Gabriola",
			nullptr,
			DWRITE_FONT_WEIGHT_BOLD,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			20.f,
			L"en-us",
			&text_format_
		);
		text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		return 0;
	}

	case VK_UP: {

		return 0;
	}
	case VK_RIGHT: {

		return 0;
	}
	case VK_DOWN: {

		return 0;
	}
	case VK_LEFT: {

		return 0;
	}


	case WM_DESTROY: {
		discard_graphics_resources();
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

		rect_ = D2D1::Rect(
				x - x / 2,
				y - y / 2,
				x + x / 2,
				y + y / 2
			);
	}
}

void MainWindow::discard_graphics_resources() {
	safe_release(&render_target_);
	safe_release(&brush_);
	safe_release(&text_format_);
	safe_release(&factory_);
	safe_release(&write_factory_);
}

void MainWindow::on_paint() {
	auto hr = create_graphics_resources();
	if(SUCCEEDED(hr)) {
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd_, &ps);

		render_target_->BeginDraw();
		render_target_->Clear(D2D1::ColorF(0.98f, 0.97f, 0.93f));
		render_target_->FillRoundedRectangle(D2D1::RoundedRect(rect_, 20.f, 20.f), brush_);
		render_target_->CreateSolidColorBrush(D2D1::ColorF(0.47f, 0.43f, 0.40f), &brush_);
		render_target_->DrawText(L"2", wcslen(L"2"), text_format_, rect_, brush_);

		render_target_->CreateSolidColorBrush(D2D1::ColorF(0.80f, 0.76f, 0.71f), &brush_);
		for (auto i = 0; i < 4; ++i) {
			for (auto j = 0; j < 4; ++j) {
				auto _slot = D2D1::RectF(rect_.left + 100.f * j, rect_.top + 100.f * i, rect_.left + 100.f * j + 50.f, rect_.top + 100.f * i + 50.f);
				render_target_->FillRoundedRectangle(D2D1::RoundedRect(_slot, 5.f, 5.f), brush_);
			}
		}
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
			const D2D1_COLOR_F color = D2D1::ColorF(0.73f, 0.68f, 0.63f);
			hr = render_target_->CreateSolidColorBrush(color, &brush_);
			if(SUCCEEDED(hr)) {
				calculate_layout();
			}
		}
	}
	return hr;
}
