#include <Windows.h>
#include <intsafe.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#include "Block.h"

std::map<std::string, Block_Color> kBlockColor =
{
		std::make_pair("0", Block_Color(205, 193, 180)),
		std::make_pair("2", Block_Color(238, 228, 218)),
		std::make_pair("4", Block_Color(237, 224, 200)),
		std::make_pair("8", Block_Color(242, 177, 121)),
		std::make_pair("16", Block_Color(245, 149, 99)),
		std::make_pair("32", Block_Color(246, 124, 95)),
		std::make_pair("64", Block_Color(245, 98, 61)),
		std::make_pair("128", Block_Color(237,207,114)),
		std::make_pair("256", Block_Color(237,204,97)),
		std::make_pair("512", Block_Color(237,200,80)),
		std::make_pair("1024", Block_Color(237,197,63)),
		std::make_pair("2048", Block_Color(237,194,45)),
};


D2D1::ColorF Block_Color::get_color_f() const {

	return color_;
}

Block::Block():
	value_(0),
	is_moving_(false){}

Block::Block(DWORD _value) :
	value_(_value),
	is_moving_(false){}

Block& Block::operator+=(const Block& _rhs) {
	value_ += _rhs.value_;

	return *this;
}

Block::operator int() const {
	return value_;
}

bool Block::operator==(const Block& _rhs) const {
	return value_ == _rhs.value_;
}

bool Block::operator!=(const Block& _rhs) const {
	return !((*this) == _rhs);
}

void Block::reset() {
	value_ = 0;
	is_moving_ = false;
}

DWORD Block::get_value() const {
	return value_;
}

bool Block::get_is_moving() const {
	return is_moving_;
}

void Block::set_is_moving(bool _is_moving) {
	is_moving_ = _is_moving;
}

HRESULT Block::paint_block(
	ID2D1HwndRenderTarget*  _render_target,
	ID2D1SolidColorBrush*	_brush,
	IDWriteTextFormat*		_text_format,
	const D2D1_RECT_F&		_rect,
	bool is_slot) const {
	constexpr auto _block_margin = 4.f;

	wchar_t* _wcs = nullptr;
	const auto _str = std::to_string(value_);

	const auto _wrapper_top = _rect.top + _block_margin;
	const auto _wrapper_right = _rect.right - _block_margin;
	const auto _wrapper_left = _rect.left + _block_margin;
	const auto _wrapper_bottom = _rect.bottom - _block_margin;

	const auto _block_wrapper = D2D1::Rect(_wrapper_left, _wrapper_top, _wrapper_right, _wrapper_bottom);
	const auto _round_rect = D2D1::RoundedRect(_block_wrapper, 2.f, 2.f);

	if (is_slot) {
		_brush->SetColor(kBlockColor["0"].get_color_f());
	}else {
		_brush->SetColor(kBlockColor[_str].get_color_f());
	}

	_render_target->FillRoundedRectangle(_round_rect, _brush);

	if(value_ != 0 && !is_slot) {
		_wcs = new wchar_t[_str.length() + 1];
		std::mbstowcs(_wcs, _str.c_str(), _str.length() + 1);

		_brush->SetColor(kTextColor);
		_render_target->DrawText(_wcs, wcslen(_wcs), _text_format, _rect, _brush);

		delete[] _wcs;
	}
	
	return S_OK;
}

D2D1::ColorF parse_color(std::initializer_list<uint8_t> _rgb, float _alpha) {
	const float r = static_cast<float>(*(_rgb.begin() + 0)) / 255;
	const float g = static_cast<float>(*(_rgb.begin() + 1)) / 255;
	const float b = static_cast<float>(*(_rgb.begin() + 2)) / 255;

	return {r, g, b, _alpha};
}

Block_Color::Block_Color(uint8_t r, uint8_t g, uint8_t b) :
	color_(parse_color({ r, g, b })) {
}

Block_Color::Block_Color() :
	color_(D2D1::ColorF(0, 0, 0)) { }

Block operator*(int _lhs, const Block& _rhs) {
	auto _tmp = Block(_lhs * _rhs.value_);

	return _tmp;
}
