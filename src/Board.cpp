#include <array>
#include <random>
#include <initializer_list>
#include <conio.h>
#include <queue>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <timeapi.h>

#include "Board.h"
#include "Block.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "winmm.lib")

const size_t kFps = 10;

Board::position_t& Board::
	position_t::operator+=(const position_t& _rhs) {
	x += _rhs.x;
	y += _rhs.y;

	return *this;
}

Board::position_t Board::
	position_t::operator-() const {
	auto _tmp = position_t((-x), (-y));

	return _tmp;
}

Board::position_t& Board::
	position_t::operator-=(const position_t& _rhs) {
	*this += (-_rhs);

	return *this;
}

bool Board::position_t::operator==(const position_t& _rhs) const {
	return x == _rhs.x && y == _rhs.y;
}

Board::Board() :
	score_(0),
	is_failed(false),
	is_full(false),
	rect_(D2D1::Rect(0, 0, 0, 0)) {

	map([&](Block& _slot,  position_t) {
		_slot = Block(0);
	});
}

Board::~Board() {
	release_resource();
}

Block& Board::operator[](position_t pos) {

	return board_[pos.y][pos.x];
}

Block Board::operator[](position_t pos) const {
	return board_[pos.y][pos.x];
}

void Board::generate_rand() {
	const auto _pos_arr = get_valid_slots();
	if (is_full) return;
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
	if(is_mergable(_pos, _direction)) {
		(*this)[_pos + _direction.second] += (*this)[_pos];
		(*this)[_pos].reset();

		score_ += (*this)[_pos + _direction.second].get_value();
	}
}

void Board::update(const direction_t& _direction, HWND _hwnd) {
	auto _pre= board_;

	map([&](Block&, const position_t& _pos) {
		move(_pos, _direction);
	}, _direction.second);

	map([&](Block&, position_t _pos) {
		merge(_pos, _direction);
	}, -_direction.second);
	map([&](Block&, position_t _pos) {
		move(_pos, _direction);
	}, _direction.second);

	auto _diff_mat = calculate_diff(_pre, board_, _direction);
	transform_mat(_pre, _diff_mat, _hwnd, _direction);
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

D2D1_SIZE_F Board::transform(
	const D2D1_RECT_F& _src,
	const Block& _pre_block,
	const position_t& _diff,
	D2D1_SIZE_F _step) const {

	auto _move_step = _src.bottom - _src.top;
	auto _x_step = _diff.x * _move_step / kFps + _step.width;
	auto _y_step = _diff.y * _move_step / kFps + _step.height;

	auto _rect = D2D1::Rect(
		_src.left + _x_step,
		_src.top + _y_step,
		_src.right + _x_step,
		_src.bottom + _y_step);

	render_target_->BeginDraw();

	_pre_block.paint_block(render_target_, trans_brush_, text_format_, _rect);

	render_target_->EndDraw();

	return { _x_step, _y_step };
}

void Board::transform_mat(
	const grid_t& _pre_grid,
	const std::array<std::array<position_t, kEdgeLen>, kEdgeLen>& _diff_mat,
	HWND _hwnd,
	const direction_t& _direction) {
	constexpr auto _margin = 7.f;
	const auto _wrapper_rect = D2D1::Rect(
		rect_.left + _margin,
		rect_.top + _margin,
		rect_.right - _margin,
		rect_.bottom - _margin);

	std::array<std::array<D2D1_SIZE_F, kEdgeLen>, kEdgeLen> _step_mat{};
	for (auto i : _step_mat) {
		i.fill({ 0.f, 0.f });
	}

	for (auto i = 0; i < kFps; ++i) {

		map([&](const Block&, const position_t& _pos) {
			if (_diff_mat[_pos.y][_pos.x] == position_t(0, 0)) return;

			auto _start_time = timeGetTime();

			const auto block_pos = calculate_block_pos(_wrapper_rect, _pos);
			_step_mat[_pos.y][_pos.x] = transform(
				block_pos,
				_pre_grid[_pos.y][_pos.x],
				_diff_mat[_pos.y][_pos.x],
				_step_mat[_pos.y][_pos.x]
			);

			/*on_paint();*/

			}, -_direction.second);

	}

	map([&](const Block&, const position_t& _pos) {
		if (_diff_mat[_pos.y][_pos.x] == position_t(0, 0)) return;
		(*this)[_pos + _diff_mat[_pos.y][_pos.x]].set_is_moving(false);
	});
}

bool Board::failed() {
	if(get_valid_slots().empty()) {
		is_failed = true;
		map([&](Block&, position_t _pos) {
			auto _next_right = _pos + direction.at("kRight");
			auto _next_down = _pos + direction.at("kDown");

			if (!is_out_of_range(_next_right)) {
				if (is_same(_pos, _next_right)) {
					is_failed = false;
					return;
				}
			}
			if (!is_out_of_range(_next_down)) {
				if (is_same(_pos, _next_down)) {
					is_failed = false;
					return;
				}
			}
		});
	}
	return is_failed;
}

bool Board::get_failed() {

	return is_failed;
}

bool Board::is_operable(const std::string& _arrow_key) {
	auto _direction = std::make_pair(_arrow_key, direction.at(_arrow_key));
	bool movable = false;

	map([&](const Block&, const position_t& _pos) {
		if(is_movable(_pos, _direction)) {
			movable = true;
		}
		}, _direction.second);

	if (movable) return true;

	bool mergable = false;
	map([&](const Block&, const position_t& _pos) {
		if (is_mergable(_pos, _direction)) {
			mergable = true;
		}
		}, _direction.second);

	return mergable;
}

HRESULT Board::init_paint(const HWND _hwnd) {
	if(FAILED(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&board_factory_
		))) {
		return -1;
	}
	// Graphics Resources
	RECT _rect;
	GetClientRect(_hwnd, &_rect);
	D2D1_SIZE_U _size = D2D1::SizeU(_rect.right, _rect.bottom);
	if (board_brush_ == nullptr) {

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

	if(trans_brush_ == nullptr) {
		if (FAILED(
			render_target_->CreateSolidColorBrush(kBoardColor, &trans_brush_)
		)) {
			return -1;
		}
	}

	return S_OK;
}

HRESULT Board::on_paint() {
	calculate_center();

	PAINTSTRUCT ps;
	HRESULT hr = S_OK;

	const auto _round_rect =  D2D1::RoundedRect(rect_, kRadius, kRadius);

	render_target_->BeginDraw();

	board_brush_->SetColor(kBoardColor);
	render_target_->Clear(kBackGroundColor);
	render_target_->FillRoundedRectangle(_round_rect, board_brush_);

	paint_slot();

	paint_block_mat();

	paint_score();

	hr = render_target_->EndDraw();

	return hr;
}

HRESULT Board::failed_paint() {

	board_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 0.5));
	auto _mask = render_target_->GetSize();
	render_target_->FillRectangle(D2D1::Rect(0.f, 0.f, _mask.width, _mask.height), board_brush_);

	board_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	auto _text_wrapper = D2D1::Rect(_mask.width / 6.f, _mask.height / 4.f, _mask.width * 5 / 6, _mask.height * 3 / 4);
	render_target_->DrawText(L"Game Over", wcslen(L"Game Over"), text_format_, _text_wrapper, board_brush_);

	return S_OK;
}

HRESULT Board::paint_block_mat() {

	HRESULT hr = S_OK;

	constexpr auto _margin = 7.f;
	const auto _wrapper_rect = D2D1::Rect(
		rect_.left + _margin,
		rect_.top + _margin,
		rect_.right - _margin,
		rect_.bottom - _margin);

	for (auto i = 0; i < kEdgeLen; ++i)
		for (auto j = 0; j < kEdgeLen; ++j) {
			auto _block = (*this)[position_t(j, i)];
			if (_block == 0) continue;
			if (_block.get_is_moving()) continue;

			const auto _block_pos = calculate_block_pos(_wrapper_rect, position_t(j, i));
			_block.paint_block(render_target_, trans_brush_, text_format_, _block_pos);
		}

	return hr;
}

HRESULT Board::paint_score() {

	board_brush_->SetColor(kBoardColor);
	auto _size = render_target_->GetSize();
	auto _score_table = D2D1::Rect(_size.width - 130.f, 10.f, _size.width - 5.f, 60.f);
	render_target_->FillRoundedRectangle(D2D1::RoundedRect(_score_table, 5.f, 5.f), board_brush_);

	board_brush_->SetColor(kTextColor);

	auto _score = std::to_string(score_);
	wchar_t* _wcs = new wchar_t[_score.length() + 1];
	std::mbstowcs(_wcs, _score.c_str(), _score.length() + 1);

	auto _top_margin = 10.f;
	auto _score_text_wrapper = D2D1::Rect(
		_score_table.left,
		_score_table.top + _top_margin,
		_score_table.right,
		_score_table.bottom
	);
	auto _score_value_wrapper = D2D1::Rect(
		_score_table.left,
		_score_table.top + 2 * _top_margin,
		_score_table.right,
		_score_table.bottom
	);

	render_target_->DrawText(L"Score:\n", wcslen(L"Score:\n"), text_format_, _score_text_wrapper, board_brush_);
	render_target_->DrawText(_wcs, wcslen(_wcs), text_format_, _score_value_wrapper, board_brush_);

	delete[] _wcs;

	return S_OK;
}

HRESULT Board::paint_slot() {
	HRESULT hr = S_OK;

	constexpr auto _margin = 7.f;
	const auto _wrapper_rect = D2D1::Rect(
		rect_.left + _margin,
		rect_.top + _margin,
		rect_.right - _margin,
		rect_.bottom - _margin);

	for (auto i = 0; i < kEdgeLen; ++i)
		for (auto j = 0; j < kEdgeLen; ++j) {
			const auto block_pos = calculate_block_pos(_wrapper_rect, position_t(j, i));
			(*this)[position_t(j, i)].paint_block(render_target_, board_brush_, text_format_, block_pos, true);
		}


	return hr;
}

HRESULT Board::handle_key(std::string _arrow_key, HWND _hwnd) {
	auto _direction = std::make_pair(_arrow_key, direction.at(_arrow_key));

	update(_direction, _hwnd);

	return S_OK;
}

HRESULT Board::release_resource() {
	safe_release(&board_brush_);
	safe_release(&trans_brush_);
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
	is_full = _pos_arr.empty();

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

// copy board_ before move
// move blocks
// use stack to find same block
// get diff of cpBoard and board_
// ret grid_t;
// transforming through matrix and making animation;
std::array<std::array<Board::position_t, kEdgeLen>, kEdgeLen> Board::calculate_diff(
	const grid_t& _pre_grid,
	const grid_t& _cur_grid,
	const direction_t& _direction) const {
	std::array<std::array<Board::position_t, kEdgeLen>, kEdgeLen> _diff_mat{};
	std::array<position_t, kEdgeLen> _row{};
	std::queue<position_t> _pre_queue;
	std::queue<position_t> _cur_queue;

	_row.fill({ 0, 0 });
	_diff_mat.fill(_row);

	position_t _start_pos(0, 0);
	auto _normal_vector = get_normal_vector(_direction);
	if (_normal_vector.y == -1 || _normal_vector.x == -1) {
		_start_pos = { kEdgeLen - 1, kEdgeLen - 1 };
	}

	for (auto i = 0; i < kEdgeLen; ++i) {
		// _pre queue of position
		for (auto j = 0; j < kEdgeLen; ++j) {
			auto _pre = _start_pos + i * _normal_vector + j * _direction.second;

			if (_pre_grid[_pre.y][_pre.x] != 0) {
				_pre_queue.push(_pre);
			}
		}

		// _cur queue of position
		for (auto j = 0; j < kEdgeLen; ++j) {
			auto _cur = _start_pos + i * _normal_vector + j * _direction.second;

			if (_cur_grid[_cur.y][_cur.x] != 0) {
				_cur_queue.push(_cur);
			}
		}

		// comparing queue and making diff
		while (!_pre_queue.empty()) {

			auto _pre_pos = _pre_queue.front();
			auto _cur_pos = _cur_queue.front();
			auto _pre_block = _pre_grid[_pre_pos.y][_pre_pos.x];
			auto _cur_block = _cur_grid[_cur_pos.y][_cur_pos.x];

			if ( _pre_block == _cur_block ) {
				_diff_mat[_pre_pos.y][_pre_pos.x] = _cur_pos - _pre_pos;
				_pre_queue.pop();
				_cur_queue.pop();
				continue;
			}
			if ( 2 * _pre_block == _cur_block) {
				_diff_mat[_pre_pos.y][_pre_pos.x] = _cur_pos - _pre_pos;
				_pre_queue.pop();

				_pre_pos = _pre_queue.front();
				_diff_mat[_pre_pos.y][_pre_pos.x] = _cur_pos - _pre_pos;

				_pre_queue.pop();
				_cur_queue.pop();
			}
		}
	}

	return _diff_mat;
}

size_t Board::get_rand(size_t _max) {
	std::random_device _rand;
	std::mt19937 _rng(_rand());
	const std::uniform_int_distribution<size_t> _dist(0, _max);

	return _dist(_rng);
}

Board::position_t Board::get_normal_vector(const direction_t& _direction) const {

	return { _direction.second.y, _direction.second.x };
}

bool Board::is_movable(const position_t& _tar, const direction_t& _direction) {
	auto _next = _tar + _direction.second;

	if (is_zero(_tar)) {
		(*this)[_tar].set_is_moving(false);
		return false;
	}
	if (is_edge(_tar, _direction)) {
		/*(*this)[_tar].set_is_moving(false);*/
		return false;
	}
	if (is_zero(_next)){
		(*this)[_tar].set_is_moving(true);
		return true;
	}
	if (is_movable(_next, _direction)) {
		move(_tar + _direction.second, _direction);
		(*this)[_tar].set_is_moving(true);

		return true;
	}
	(*this)[_tar].set_is_moving(false);
	return false;
}

bool Board::is_mergable(const position_t& _tar, const direction_t& _direction) {
	if (is_zero(_tar)) return false;
	if (!is_edge(_tar, _direction) &&
		is_same(_tar + _direction.second, _tar))
		return true;

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
		return _tar.x == 0;
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
