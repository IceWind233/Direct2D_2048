#ifndef BLOCK_H
#define BLOCK_H

#include <map>
#include <string>

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

/*struct rgb8 {
	short r_, g_, b_;
	rgb8(short r, short g, short b) :
		r_(r), g_(g), b_(b) { }
	rgb8() :
		r_(0), g_(0), b_(0)	{ }
};*/

D2D1::ColorF parse_color(std::initializer_list<uint8_t> _rgb, float _alpha = 1.f);

/*D2D1::ColorF parse_color(rgb8, float _alpha = 1.f);*/

const auto kTextColor = D2D1::ColorF(parse_color({ 119, 110, 101 }));


/*
auto kBlockColor =
	std::map<std::string, rgb8>{
		 std::make_pair("0", rgb8{205, 193, 180}),
		 std::make_pair("2", rgb8{238, 228, 218}),
		 std::make_pair("4", rgb8{237, 224, 200}),
		 std::make_pair("8", rgb8{242, 177, 121}),
		 std::make_pair("16", rgb8{245, 149, 99}),
		 std::make_pair("32", rgb8{246, 124, 95}),
		 std::make_pair("64", rgb8{245, 98, 61}),
	};
	*/

template <typename Ty>
void safe_release(Ty** ppT) {
	if (*ppT) {
		(*ppT)->Release();
		(*ppT) = nullptr;
	}
}

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

public:
	void reset();

	HRESULT paint_block(
		ID2D1HwndRenderTarget* _render_target,
		ID2D1SolidColorBrush* _brush,
		IDWriteTextFormat* _text_format,
		D2D1_RECT_F _rect);
	
private:
	DWORD value_;
};

#endif