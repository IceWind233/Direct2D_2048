#include <array>
#include <random>
#include <conio.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#include "Board.h"
#include "Block.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

Board::position_t& Board::
	position_t::operator+=(const position_t& _rhs) {
	x += _rhs.x;
	y += _rhs.y;

	return *this;
}

Board::position_t& Board::
	position_t::operator-() const {
	auto _tmp = position_t((-x), (-y));

	return _tmp;
}

Board::position_t& Board::
	position_t::operator-=(const position_t& _rhs) {
	*this += (-_rhs);

	return *this;
}

//TODO:: Board Size
Board::Board() :
	score(0),
	rect_(D2D1::Rect(0, 0, 0, 0)) {

	map([&](Block& _slot,  position_t) {
		_slot = Block(0);
	});

}

Board::~Board() {
	release_resource();
}

Block& Board::operator[](position_t pos) {

	return board[pos.y][pos.x];
}

void Board::generate_rand() {
	const auto _pos_arr = get_valid_slots();
	const auto _rand = get_rand(_pos_arr.size() - 1);
	const auto _pos = _pos_arr[_rand];

	(*this)[_pos] = Block(2);
}

void Board::move(const position_t& _pos, const direction_t& _direction) {
	if (is_movable(_pos, _direction)) {
		(*this)[_pos + _direction.second] = (*this)[_pos];

		(*this)[_pos].reset(); 
	}
}

void Board::merge(const position_t& _pos, const direction_t& _direction) {
	if(is_zero(_pos)) return;
	if (!is_edge(_pos, _direction) && 
		is_same(_pos + _direction.second, _pos)) {

		(*this)[_pos + _direction.second] += (*this)[_pos];

		(*this)[_pos].reset();
	}
}

void Board::update(const direction_t& _direction) {
	map([&](Block&, position_t _pos) {
		move(_pos, _direction);
	}, _direction.second);
	map([&](Block&, position_t _pos) {
		merge(_pos, _direction);
	}, -_direction.second);
	map([&](Block&, position_t _pos) {
		move(_pos, _direction);
	}, _direction.second);
}

void Board::calculate_center() {
	auto _window_size = render_target_->GetSize();
	D2D1_POINT_2F _window_center = { _window_size.width / 2,_window_size.height / 2 };

	rect_ = D2D1::Rect(
		_window_center.x - 150.f,
		_window_center.y - 150.f,
		_window_center.x + 150.f,
		_window_center.y + 150.f
	);
}


//TODO:: Lambda function return or break;
bool Board::failed() {
	/*if(get_valid_slots().empty()) {
		map([&](Block& _slot, position_t _pos) {
			map_all_direction([&](direction_t _direction) {
				if (!is_out_of_range(_pos + _direction.second)) {
					if (is_same(_pos, _pos + _direction.second)) {
						return false;
					}
				}
			});
		});
		return true;
	}*/
	return false;
}

HRESULT Board::init_paint(const HWND _hwnd) {
	if(FAILED(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&board_factory_
		))) {
		return -1;
	}
	// Graphics Resources
	if (board_brush_ == nullptr) {
		RECT _rect;
		GetClientRect(_hwnd, &_rect);
		D2D1_SIZE_U _size = D2D1::SizeU(_rect.right, _rect.bottom);

		if (FAILED(board_factory_->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(_hwnd, _size),
			&render_target_
		))) {
			return -1;
		}

		if (FAILED(DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&write_factory_)
		))) {
			return -1;
		}

		if(FAILED(write_factory_->CreateTextFormat(
			kFontFamily,
			nullptr,
			DWRITE_FONT_WEIGHT_BOLD,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			kBlockFontSize,
			L"en-us",
			&text_format_
		))) {
			return -1;
		}

		text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		if (FAILED(
			render_target_->CreateSolidColorBrush(kBoardColor, &board_brush_)
		)) {
			return -1;
		}
	}

	return S_OK;
}

HRESULT Board::on_paint(const HWND _hwnd) {
	calculate_center();

	PAINTSTRUCT ps;
	HRESULT hr = S_OK;

	const auto _round_rect =  D2D1::RoundedRect(rect_, kRadius, kRadius);

	BeginPaint(_hwnd, &ps);
	render_target_->BeginDraw();

	board_brush_->SetColor(kBoardColor);
	render_target_->Clear(kBackGroundColor);
	render_target_->FillRoundedRectangle(_round_rect, board_brush_);

	hr = render_target_->EndDraw();

	EndPaint(_hwnd, &ps);

	paint_block_mat(_hwnd);

	return hr;
}

HRESULT Board::paint_block_mat(HWND _hwnd) {

	PAINTSTRUCT ps;
	HRESULT hr = S_OK;

	constexpr auto _margin = 7.f;
	const auto _wrapper_rect = D2D1::Rect(
		rect_.left + _margin,
		rect_.top + _margin,
		rect_.right - _margin,
		rect_.bottom - _margin);

	BeginPaint(_hwnd, &ps);

	for (auto i = 0; i < kEdgeLen; ++i)
		for (auto j = 0; j < kEdgeLen; ++j) {
			const auto block_pos = calculate_block_pos(_wrapper_rect, position_t(j, i));
			(*this)[position_t(j, i)].paint_block(render_target_, board_brush_, text_format_, block_pos);
		}

	EndPaint(_hwnd, &ps);

	return hr;
}

HRESULT Board::handle_key(std::string _arrow_key, HWND _hwnd) {

	auto _direction = std::make_pair(_arrow_key, direction.at(_arrow_key));
	update(_direction);

	PAINTSTRUCT _ps;
	wchar_t* _wcs = new wchar_t[_arrow_key.length() + 1];
	std::mbstowcs(_wcs, _arrow_key.c_str(), _arrow_key.length() + 1);
	auto _tmp_rect = D2D1::Rect(500, 500, 550, 550);

	BeginPaint(_hwnd, &_ps);
	render_target_->BeginDraw();

	board_brush_->SetColor(D2D1::ColorF(0.f, 1.f, 0.f));
	render_target_->FillRectangle(_tmp_rect, board_brush_);

	board_brush_->SetColor(D2D1::ColorF(1.f, 0.f, 0.f));
	render_target_->DrawText(_wcs, wcslen(_wcs), text_format_, _tmp_rect, board_brush_);

	render_target_->EndDraw();
	EndPaint(_hwnd, &_ps);
	
	return S_OK;
}

HRESULT Board::release_resource() {
	safe_release(&board_brush_);
	safe_release(&board_factory_);
	safe_release(&render_target_);
	safe_release(&text_format_);
	safe_release(&write_factory_);

	return S_OK;
}

std::vector<Board::position_t> Board::get_valid_slots() {
	std::vector<position_t> _pos_arr;

	map([&](const Block& _slot, position_t _pos) {
		if (static_cast<int>(_slot) == 0) {
			_pos_arr.emplace_back(_pos);
		}
	});

	return _pos_arr;
}

D2D1_RECT_F Board::calculate_block_pos(D2D1_RECT_F init_rect, position_t _pos) {
	const auto _edge = (init_rect.right - init_rect.left) / kEdgeLen;
	const auto _top = _pos.y * _edge + init_rect.top;
	const auto _left = _pos.x * _edge + init_rect.left;
	const auto _right = _left + _edge;
	const auto _bottom = _top + _edge;

	return D2D1::Rect(_left, _top, _right, _bottom);
}

size_t Board::get_rand(size_t _max) {
	std::random_device _rand;
	std::mt19937 _rng(_rand());
	const std::uniform_int_distribution<size_t> _dist(0, _max);

	return _dist(_rng);
}

bool Board::is_movable(const position_t& _tar, const direction_t& _direction) {
	auto _next = _tar + _direction.second;

	if (is_zero(_tar)) {
		return false;
	}
	if (is_edge(_tar, _direction)) {
		return false;
	}
	if (is_zero(_next)){
		return true;
	}
	if (is_movable(_next, _direction)) {
		move(_tar + _direction.second, _direction);

		return true;
	}

	return false;
}

bool Board::is_edge(const position_t& _tar, const direction_t& _direction) {
	if (_direction.first == "kUp") {
		return _tar.y == 0;
	}
	if (_direction.first == "kRight") {
		return _tar.x == kEdgeLen - 1;
	}
	if (_direction.first == "kDown") {
		return _tar.y == kEdgeLen - 1;
	}
	if (_direction.first == "kLeft") {
		return _tar.y == 0;
	}
	return false;
}

bool Board::is_same(const position_t& _lhs, const position_t& _rhs) {

	return (*this)[_lhs] == (*this)[_rhs];
}

bool Board::is_out_of_range(const position_t& _pos) {
	if (_pos.x >= kEdgeLen) return true;
	if (_pos.y >= kEdgeLen) return true;
	if (_pos.x < 0) return true;
	if (_pos.y < 0) return true;
	return false;
}

bool Board::is_zero(const position_t& _pos) {

	return (*this)[_pos] == 0;
}


