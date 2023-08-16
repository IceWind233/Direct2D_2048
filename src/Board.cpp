#include <array>
#include <random>
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

constexpr size_t kFps = 5;
const std::map<std::string, Board::position_t> directions = {
	{"kUp",		Board::position_t(0, -1)},
	{"kRight",	Board::position_t(1, 0)},
	{"kDown",	Board::position_t(0, 1)},
	{"kLeft",	Board::position_t(-1, 0)},
};

/******** Board::position_t *********/

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

/************* Board ***************/

Board::Board() :
	score_(0),
	is_failed(false),
	is_full(false){

	map([&](Block& _slot,  position_t) {
		_slot = Block(0);
	});
}

Block& Board::operator[](position_t pos) {

	return board_[pos.y][pos.x];
}

Block Board::operator[](position_t pos) const {
	return board_[pos.y][pos.x];
}

bool Board::get_failed() const {

	return is_failed;
}

void Board::set_failed(bool _is_failed) {
	is_failed = _is_failed;
}

bool Board::get_full() const {
	return is_full;
}

void Board::set_full(bool _is_full) {
	is_full = _is_full;
}

const Board::grid_t& Board::get_board() {
	return board_;
}

Board::grid_t& Board::set_board() {
	return board_;
}

size_t Board::get_score() const {
	return score_;
}

void Board::set_score(size_t _score) {
	score_ = _score;
}


/************ BoardView **************/
// Packaging D2D Class 

BoardView::Rect_F::Rect_F() :
	Rect_F(0, 0, 0, 0) {}

BoardView::Rect_F::Rect_F(
	float _left,
	float _top,
	float _right,
	float _bottom) :
	left_(_left),
	top_(_top),
	right_(_right),
	bottom_(_bottom) {}

BoardView::Rect_F::Rect_F(const D2D1_RECT_F& _rect) :
	Rect_F(_rect.left, _rect.top, _rect.right, _rect.bottom) {}

D2D1_RECT_F BoardView::Rect_F::get_self() const {
	return D2D1_RECT_F(D2D1::RectF(left_, top_, right_, bottom_));
}

BoardView::Size_U::Size_U() :
	Size_U(0 ,0) {}

BoardView::Size_U::Size_U(uint32_t _width, uint32_t _height) :
	width_(_width), height_(_height) {}

BoardView::Size_U::Size_U(const D2D1_SIZE_U& _size) :
	Size_U(_size.width, _size.height) {}

D2D1_SIZE_U BoardView::Size_U::get_self() const {
	return D2D1::SizeU(width_, height_);
}

BoardView::Size_F::Size_F() :
	Size_F(0.f, 0.f) {}

BoardView::Size_F::Size_F(float _width, float _height) :
	width_(_width), height_(_height) {}

BoardView::Size_F::Size_F(const D2D1_SIZE_F& _size) :
	Size_F(_size.width, _size.height) {}

D2D1_SIZE_F BoardView::Size_F::get_self() const {
	return D2D1::SizeF(width_, height_);
}

BoardView::BoardView() : rect_(0, 0, 0, 0) {}

/****************************************/

BoardView::BoardView(HWND _hwnd) {
	init_paint(_hwnd);
}

BoardView::~BoardView() {
	release_resource();
}
	
HRESULT BoardView::init_paint(const HWND& _hwnd) {
	if (FAILED(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		board_factory_.set()
	))) {
		return -1;
	}
	// Graphics Resources
	RECT _rect;
	GetClientRect(_hwnd, &_rect);
	const auto _size = Size_U(_rect.right, _rect.bottom);
	if (board_brush_.get() != nullptr) {

		if (FAILED(board_factory_->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(_hwnd, _size.get_self()),
			render_target_.set()
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

		if (FAILED(write_factory_->CreateTextFormat(
			kFontFamily,
			nullptr,
			DWRITE_FONT_WEIGHT_BOLD,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			kBlockFontSize,
			L"en-us",
			text_format_.set()
		))) {
			return -1;
		}

		text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		if (FAILED(
			render_target_->CreateSolidColorBrush(kBoardColor, board_brush_.set())
		)) {
			return -1;
		}
	}

	return S_OK;
}

HRESULT BoardView::on_paint(const Board& _board) {
	HRESULT hr = S_OK;

	render_target_->BeginDraw();

	hr = paint_static(_board);

	if (_board.get_failed()) {
		hr = failed_paint();
	}

	hr = render_target_->EndDraw();

	return hr;
}

HRESULT BoardView::failed_paint() {
	board_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 0.5));
	const auto _mask = render_target_->GetSize();
	render_target_->FillRectangle(D2D1::Rect(0.f, 0.f, _mask.width, _mask.height), board_brush_.get());

	board_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	const auto _text_wrapper = D2D1::Rect(_mask.width / 6.f, _mask.height / 4.f, _mask.width * 5 / 6, _mask.height * 3 / 4);
	render_target_->DrawText(L"Game Over", wcslen(L"Game Over"), text_format_.get(), _text_wrapper, board_brush_.get());

	return S_OK;
}

HRESULT BoardView::paint_static(const Board& _board) {
	calculate_center();

	HRESULT hr = S_OK;
	const auto _round_rect = D2D1::RoundedRect(rect_.get_self(), kRadius, kRadius);

	board_brush_->SetColor(kBoardColor);
	render_target_->Clear(kBackGroundColor);
	render_target_->FillRoundedRectangle(_round_rect, board_brush_.get());

	hr = paint_slot(_board);

	hr = paint_block_mat(_board);

	hr = paint_score(_board.get_score());

	return hr;
}

HRESULT BoardView::paint_block_mat(const Board& _board) const {

	HRESULT hr = S_OK;

	constexpr auto _margin = 7.f;
	const auto _wrapper_rect = Rect_F(
		rect_.left_ + _margin,
		rect_.top_ + _margin,
		rect_.right_ - _margin,
		rect_.bottom_ - _margin);

	for (auto i = 0; i < kEdgeLen; ++i)
		for (auto j = 0; j < kEdgeLen; ++j) {
			auto _block = _board[Board::position_t(j, i)];
			if (_block == 0) continue;
			if (_block.get_is_moving()) continue;

			const auto _block_pos = calculate_block_pos(_wrapper_rect, Board::position_t(j, i));
			_block.paint_block(render_target_.get(), board_brush_.get(), text_format_.get(), _block_pos.get_self());
		}

	return hr;
}

HRESULT BoardView::paint_score(size_t _score) const {

	board_brush_->SetColor(kBoardColor);
	const auto _size = render_target_->GetSize();
	const auto _score_table = Rect_F(_size.width - 130.f, 10.f, _size.width - 5.f, 60.f);
	render_target_->FillRoundedRectangle(D2D1::RoundedRect(_score_table.get_self(), 5.f, 5.f), board_brush_.get());

	board_brush_->SetColor(kTextColor);
	
	const auto _score_str = std::to_string(_score);
	auto* _wcs = new wchar_t[_score_str.length() + 1];
	std::mbstowcs(_wcs, _score_str.c_str(), _score_str.length() + 1);

	constexpr auto _top_margin = 10.f;
	const auto _score_text_wrapper = set_rect_margin(
		_score_table,
		0.f,
		_top_margin, 
		0.f,
		0.f);
	const auto _score_value_wrapper = set_rect_margin(
		_score_table,
		0.f,
		2 * _top_margin,
		0.f,
		0.f);

	render_target_->DrawText(
		L"Score:\n", 
		wcslen(L"Score:\n"),
		text_format_.get(),
		_score_text_wrapper.get_self(), 
		board_brush_.get());

	render_target_->DrawText(
		_wcs,
		wcslen(_wcs),
		text_format_.get(),
		_score_value_wrapper.get_self(),
		board_brush_.get());

	delete[] _wcs;

	return S_OK;
}

HRESULT BoardView::paint_slot(const Board& _board) const {
	HRESULT hr = S_OK;

	constexpr auto _margin = 7.f;
	const auto _wrapper_rect = set_rect_margin(rect_, _margin);

	for (auto i = 0; i < kEdgeLen; ++i)
		for (auto j = 0; j < kEdgeLen; ++j) {
			const auto block_pos = calculate_block_pos(_wrapper_rect, Board::position_t(j, i));
			_board[Board::position_t(j, i)].
				paint_block(
					render_target_.get(),
					board_brush_.get(),
					text_format_.get(),
					block_pos.get_self(), 
					true);
		}
	return hr;
}

HRESULT BoardView::release_resource() {
	render_target_.reset();
	board_brush_.reset();
	board_factory_.reset();
	text_format_.reset();
	write_factory_.reset();

	return S_OK;
}

BoardView::Size_F BoardView::transform(
	const Rect_F& _src,
	const Block& _pre_block,
	const Board::position_t& _diff,
	Size_F _step) const {
	const auto _move_step = _src.bottom_ - _src.top_;
	auto _x_step = _diff.x * _move_step / kFps + _step.width_;
	auto _y_step = _diff.y * _move_step / kFps + _step.height_;

	const auto _rect = set_rect_margin(
		_src,
		_x_step, 
		_y_step, 
		-_x_step, 
		-_y_step);

	_pre_block.paint_block(render_target_.get(), board_brush_.get(), text_format_.get(), _rect.get_self());

	return { _x_step, _y_step };
}

void BoardView::transform_mat(
	const Board::grid_t& _pre_grid,
	Board& _board,
	const std::array<std::array<Board::position_t, kEdgeLen>, kEdgeLen>& _diff_mat,
	const Board::direction_t& _direction) {
	constexpr auto _margin = 7.f;
	const auto _wrapper_rect = set_rect_margin(rect_, _margin);

	std::array<std::array<Size_F, kEdgeLen>, kEdgeLen> _step_mat{};
	for (auto i : _step_mat) {
		i.fill({ 0.f, 0.f });
	}

	for (auto i = 0; i < kFps; ++i) {
		render_target_->BeginDraw();

		paint_static(_board);

		_board.map([&](const Block&, const Board::position_t& _pos) {
			if (_diff_mat[_pos.y][_pos.x] == Board::position_t(0, 0)) return;

			const auto block_pos = calculate_block_pos(_wrapper_rect, _pos);
			_step_mat[_pos.y][_pos.x] = transform(
				block_pos,
				_pre_grid[_pos.y][_pos.x],
				_diff_mat[_pos.y][_pos.x],
				_step_mat[_pos.y][_pos.x]
			);

			}, -_direction.second);

		render_target_->EndDraw();
		set_time_delay_ms(20);
	}

	_board.map([&](Block&, const Board::position_t& _pos) {
		_board[_pos].set_is_moving(false);
		});
}

BoardView::Rect_F BoardView::set_rect_margin(
	Rect_F _rect,
	float _left_margin,
	float _top_margin,
	float _right_margin,
	float _bottom_margin) {

	return {
		_rect.left_ + _left_margin,
		_rect.top_ + _top_margin,
		_rect.right_ - _right_margin,
		_rect.bottom_ - _bottom_margin
	};
}

BoardView::Rect_F BoardView::set_rect_margin(Rect_F _rect, float _margin) const {
	return set_rect_margin(
		_rect,
		_margin,
		_margin,
		_margin,
		_margin);
}

BoardView::Rect_F BoardView::calculate_block_pos(Rect_F init_rect, Board::position_t _pos) {
	const auto _edge = (init_rect.right_ - init_rect.left_) / kEdgeLen;
	const auto _top = _pos.y * _edge + init_rect.top_;
	const auto _left = _pos.x * _edge + init_rect.left_;
	const auto _right = _left + _edge;
	const auto _bottom = _top + _edge;

	return {_left, _top, _right, _bottom};
}

void BoardView::calculate_center() {
	const auto _window_size = render_target_->GetSize();
	const Size_F _window_center = { _window_size.width / 2,_window_size.height / 2 };

	rect_ = Rect_F(
		_window_center.width_ - 150.f,
		_window_center.height_ - 150.f,
		_window_center.width_ + 150.f,
		_window_center.height_ + 150.f
	);
}

void BoardView::set_time_delay_ms(size_t _ms) {
	const auto _start = timeGetTime();
	while (timeGetTime() - _start < _ms);
}

/************* BoardController *************/

BoardController::BoardController() {}

BoardController::~BoardController() {
	board_view_.~BoardView();
	board_model_.~Board();
}

void BoardController::generate_rand() {
	generate_rand(2);
}

void BoardController::generate_rand(int _val) {
	const auto _pos_arr = get_valid_slots();
	if (board_model_.get_full()) return;
	const auto _rand = get_rand(_pos_arr.size() - 1);
	const auto _pos = _pos_arr[_rand];

	board_model_[_pos] = Block(_val);
}

void BoardController::move(const Board::position_t& _pos, const Board::direction_t& _direction) {
	if (is_movable(_pos, _direction)) {
		board_model_[_pos + _direction.second] = board_model_[_pos];

		board_model_[_pos].reset();
	}
}

void BoardController::merge(const Board::position_t& _pos, const Board::direction_t& _direction) {
	if (is_mergable(_pos, _direction)) {
		board_model_[_pos + _direction.second] += board_model_[_pos];
		board_model_[_pos].reset();

		board_model_.set_score(board_model_.get_score() + board_model_[_pos + _direction.second].get_value());
	}
}

void BoardController::release_resource() {
	board_view_.release_resource();
}

bool BoardController::failed() {
	if (get_valid_slots().empty()) {
		board_model_.set_failed(true);
		board_model_.map([&](Block&, Board::position_t _pos) {
			const auto _next_right = _pos + directions.at("kRight");
			const auto _next_down = _pos + directions.at("kDown");

			if (!is_out_of_range(_next_right)) {
				if (is_same(_pos, _next_right)) {
					board_model_.set_failed(false);
					return;
				}
			}
			if (!is_out_of_range(_next_down)) {
				if (is_same(_pos, _next_down)) {
					board_model_.set_failed(false);
					return;
				}
			}
			});
	}
	return board_model_.get_failed();
}

bool BoardController::is_operable(const std::string& _arrow_key) {
	const auto _direction = std::make_pair(_arrow_key, directions.at(_arrow_key));
	bool movable = false;

	board_model_.map([&](const Block&, const Board::position_t& _pos) {
		if (is_movable(_pos, _direction)) {
			movable = true;
		}
		}, _direction.second);

	if (movable) return true;

	bool mergable = false;
	board_model_.map([&](const Block&, const Board::position_t& _pos) {
		if (is_mergable(_pos, _direction)) {
			mergable = true;
		}
		}, _direction.second);

	return mergable;
}

std::vector<Board::position_t> BoardController::get_valid_slots() {
	std::vector<Board::position_t> _pos_arr;

	board_model_.map([&](const Block& _slot, Board::position_t _pos) {
		if (static_cast<int>(_slot) == 0) {
			_pos_arr.emplace_back(_pos);
		}
		});
	board_model_.set_full(_pos_arr.empty());

	return _pos_arr;
}

void BoardController::update(const Board::direction_t& _direction) {
	const auto _pre = board_model_.get_board();

	board_model_.map([&](Block&, const Board::position_t& _pos) {
		move(_pos, _direction);
		}, _direction.second);

	board_model_.map([&](Block&, Board::position_t _pos) {
		merge(_pos, _direction);
		}, -_direction.second);
	board_model_.map([&](Block&, Board::position_t _pos) {
		move(_pos, _direction);
		}, _direction.second);

	const auto _diff_mat = calculate_diff(_pre, board_model_.get_board(), _direction);
	board_view_.transform_mat(_pre, board_model_, _diff_mat, _direction);
}

HRESULT BoardController::handle_key(const std::string& _arrow_key) {

	HRESULT hr = S_OK;
	const auto _direction = std::make_pair(_arrow_key, directions.at(_arrow_key));

	update(_direction);
	generate_rand();

	hr = board_view_.paint_block_mat(board_model_);
	hr = board_view_.paint_score(board_model_.get_score());

	failed();

	return hr;
}

HRESULT BoardController::init_paint(HWND _hwnd) {
	return board_view_.init_paint(_hwnd);
}

HRESULT BoardController::on_paint() {
	return board_view_.on_paint(board_model_);
}

std::array<std::array<Board::position_t, kEdgeLen>, kEdgeLen> BoardController::calculate_diff(
	const Board::grid_t& _pre_grid,
	const Board::grid_t& _cur_grid,
	const Board::direction_t& _direction) {
	std::array<std::array<Board::position_t, kEdgeLen>, kEdgeLen> _diff_mat{};
	std::array<Board::position_t, kEdgeLen> _row{};
	std::queue<Board::position_t> _pre_queue;
	std::queue<Board::position_t> _cur_queue;

	_row.fill({ 0, 0 });
	_diff_mat.fill(_row);

	Board::position_t _start_pos(0, 0);
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

			if (_pre_block == _cur_block) {
				_diff_mat[_pre_pos.y][_pre_pos.x] = _cur_pos - _pre_pos;
				_pre_queue.pop();
				_cur_queue.pop();
				continue;
			}
			if (2 * _pre_block == _cur_block) {
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

size_t BoardController::get_rand(size_t _max) {
	std::random_device _rand;
	std::mt19937 _rng(_rand());
	const std::uniform_int_distribution<size_t> _dist(0, _max);

	return _dist(_rng);
}

Board::position_t BoardController::get_normal_vector(const Board::direction_t& _direction) {

	return { _direction.second.y, _direction.second.x };
}

bool BoardController::is_movable(const Board::position_t& _tar, const Board::direction_t& _direction) {
	const auto _next = _tar + _direction.second;

	if (is_zero(_tar)) {
		board_model_[_tar].set_is_moving(false);
		return false;
	}
	if (is_edge(_tar, _direction)) {
		return false;
	}
	if (is_zero(_next)) {
		board_model_[_tar].set_is_moving(true);
		return true;
	}
	if (is_movable(_next, _direction)) {
		move(_tar + _direction.second, _direction);
		board_model_[_tar].set_is_moving(true);

		return true;
	}
	board_model_[_tar].set_is_moving(false);
	return false;
}

bool BoardController::is_mergable(const Board::position_t& _tar, const Board::direction_t& _direction) {
	if (is_zero(_tar)) return false;
	if (!is_edge(_tar, _direction) &&
		is_same(_tar + _direction.second, _tar))
		return true;

	return false;
}

bool BoardController::is_edge(const Board::position_t& _tar, const Board::direction_t& _direction) {
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

bool BoardController::is_same(const Board::position_t& _lhs, const Board::position_t& _rhs) {
	return board_model_[_lhs] == board_model_[_rhs];
}

bool BoardController::is_out_of_range(const Board::position_t& _pos) {
	if (_pos.x >= kEdgeLen) return true;
	if (_pos.y >= kEdgeLen) return true;
	return false;
}

bool BoardController::is_zero(const Board::position_t& _pos) {
	return board_model_[_pos] == 0;
}

bool BoardController::get_failed() const {
	return board_model_.get_failed();
}

