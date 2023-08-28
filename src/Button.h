#ifndef BUTTON_H
#define BUTTON_H

#include "Windows.h"
#include <d2d1.h>
#include <dwrite.h>

#include "ComPtr.h"

//Todo:: create button 

/*
class ButtonView {

public:

	ButtonView(
		D2D1::ColorF _button_color_,
		D2D1::ColorF _button_text_color,
		D2D1_RECT_F _button_wrapper);

public:

	HRESULT init_button(HWND _hwnd);

private:

	com_ptr<ID2D1Factory> button_factory_{};
	com_ptr<ID2D1HwndRenderTarget> render_button_{};
	com_ptr<ID2D1SolidColorBrush> button_brush_{};

	com_ptr<IDWriteTextFormat> button_text_format_{};
	com_ptr<IDWriteFactory> button_text_factory{};

	D2D1_RECT_F button_wrapper_;
	D2D1::ColorF button_color;
	D2D1::ColorF button_text_color;

};

inline ButtonView::ButtonView(
	D2D1::ColorF _button_color_,
	D2D1::ColorF _button_text_color_,
	D2D1_RECT_F _button_wrapper) :
	button_wrapper_(_button_wrapper),
	button_color(_button_color_),
	button_text_color(_button_text_color_) {}

inline HRESULT ButtonView::init_button(HWND _hwnd) {
	D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		button_factory_.set());

	const auto _button_size = D2D1_SIZE_U(button_wrapper_.right, button_wrapper_);

	if (button_brush_) {
		button_factory_->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(_hwnd, _button_size),
			render_button_.set()
		);

		DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteTextFormat),

			)
	}

}
*/

class Button {

public:

	Button();

	Button(
		wchar_t* _text,
		D2D1::ColorF _button_color_,
		D2D1::ColorF _button_text_color,
		D2D1_RECT_F _button_wrapper,
		HWND _parent_hwnd);

public:

	void create_button();

	void paint_button(ID2D1HwndRenderTarget* _render_target, ID2D1SolidColorBrush* _brush, IDWriteTextFormat* _text_format) const;

private:

	wchar_t* text_;
	D2D1_RECT_F button_wrapper_;
	D2D1::ColorF button_color;
	D2D1::ColorF button_text_color_;
	HWND parent_hwnd_; 

};

inline Button::Button(
	wchar_t* _text,
	D2D1::ColorF _button_color_,
	D2D1::ColorF _button_text_color,
	D2D1_RECT_F _button_wrapper,
	HWND _parent_hwnd) :
	text_(_text),
	button_wrapper_(_button_wrapper),
	button_color(_button_color_),
	button_text_color_(_button_text_color),
	parent_hwnd_(_parent_hwnd) { }

inline Button::Button() :
	Button(
		L"",
		D2D1::ColorF::Black,
		D2D1::ColorF::Black,
		D2D1::RectF(0.f, 0.f, 0.f, 0.f),
		nullptr
		) {}

inline void Button::create_button() {
	const auto _button_width = button_wrapper_.right - button_wrapper_.left;
	const auto _button_height = button_wrapper_.bottom - button_wrapper_.top;

	auto _button_hwnd = CreateWindow(
		L"Button",
		text_,
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		button_wrapper_.left,
		button_wrapper_.top,
		_button_width,
		_button_height,
		parent_hwnd_,
		nullptr,
		reinterpret_cast<HINSTANCE>(GetWindowLongPtr(parent_hwnd_, GWLP_HINSTANCE)),
		nullptr);

}

inline void Button::paint_button(
	ID2D1HwndRenderTarget* _render_target,
	ID2D1SolidColorBrush* _brush,
	IDWriteTextFormat* _text_format) const {

	_brush->SetColor(D2D1::ColorF(0xBAADA1));
	_render_target->FillRoundedRectangle(D2D1::RoundedRect(button_wrapper_, 3.f, 3.f), _brush);
	_brush->SetColor(D2D1::ColorF(0x79716D));
	_render_target->DrawTextW(text_, wcslen(text_), _text_format, button_wrapper_, _brush);

}

#endif
