#ifndef BLOCK_H
#define BLOCK_H

#include <map>
#include <string>

#include <Windows.h>
#include <d2d1.h>

D2D1::ColorF parse_color(std::initializer_list<uint8_t> _rgb, float _alpha = 1.f);

const auto kTextColor = D2D1::ColorF(parse_color({ 119, 110, 101 }));

class Block_Color {

	D2D1::ColorF color_;

public:
	Block_Color();

	Block_Color(uint8_t r, uint8_t g, uint8_t b);
public:

	D2D1::ColorF get_color_f() const;

};

class Block {

public:
	Block();

	Block(DWORD _value);

	Block(const Block& _other) = default;

	Block& operator=(const Block& _other) = default;

	Block(Block&& _ref) = default;

	Block& operator=(Block&& _ref) = default;

	~Block() = default;

public:

	Block& operator+=(const Block& _rhs);

	explicit operator int() const;

	bool operator==(const Block& _rhs) const;

	bool operator!=(const Block& _rhs) const;

	friend Block operator*(int _lhs, const Block& _rhs);

public:
	void reset();

	DWORD get_value() const;

	bool get_is_moving() const;

	void set_is_moving(bool _is_moving);

	HRESULT paint_block(
		ID2D1HwndRenderTarget* _render_target,
		ID2D1SolidColorBrush* _brush,
		IDWriteTextFormat* _text_format,
		const D2D1_RECT_F& _rect,
		bool is_slot=false) const;
	
private:
	uint32_t value_;

	bool is_moving_;
};

#endif