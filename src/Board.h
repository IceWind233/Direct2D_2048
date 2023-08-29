#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <map>
#include <array>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#include "Block.h"
#include "Button.h"
#include "ComPtr.h"

constexpr size_t kEdgeLen = 4;
constexpr FLOAT kRadius = 10.f;
constexpr FLOAT kBlockFontSize = 20.f;
const auto kFontFamily = L"JetBrains Mono Medium";
const D2D1::ColorF kBoardColor = D2D1::ColorF(.73f, .68f, .63f);
const D2D1::ColorF kBackGroundColor = D2D1::ColorF(.98f, .97f, .93f);


class Board {

	friend class BoardView;
	friend class BoardController;

public:

	struct position_t;

	typedef std::pair<std::string, position_t> direction_t;

	typedef std::array<std::array<Block, kEdgeLen>, kEdgeLen> grid_t;

	struct position_t {
		int x;
		int y;

		position_t(int _x, int _y)
			: x(_x), y(_y){ }

		position_t() = default;

		position_t(const position_t& _other) = default;

		position_t& operator=(const position_t& _other) = default;

		position_t(position_t&& _ref) = default;

		position_t& operator=(position_t&& _ref) = default;

		position_t& operator+=(const position_t& _rhs);

		position_t operator-() const;

		position_t& operator-=(const position_t& _rhs);

		bool operator==(const position_t& _rhs) const;

		friend position_t operator+(const position_t& lhs, const position_t& _rhs) {
			position_t tmp(lhs);

			return tmp += _rhs;
		}

		friend position_t operator-(const position_t& lhs, const position_t& _rhs) {
			position_t tmp(lhs);

			return tmp -= _rhs;
		}

		friend position_t operator*(int _lhs, const position_t& _rhs) {
			return { _rhs.x * _lhs, _rhs.y * _lhs };
		}
	};

public:

	Board();

	~Board() = default;

public:

	Block& operator[](position_t pos);

	Block operator[](position_t pos) const;

public:

	template <typename func_t>
	void map(func_t func, Board::position_t _direction = Board::position_t(0, 1)) const;

	size_t get_score() const;

	void set_score(size_t _score);

	size_t get_highest_score() const;

	void set_highest_score(size_t _score);

	size_t get_file() const;

	void set_file(size_t _highest_score) const;

	bool get_failed() const;

	void set_failed(bool _is_failed);

	bool get_full() const;

	void set_full(bool _is_full);

	const grid_t& get_board();

	grid_t& set_board();

private:

	grid_t board_;

	size_t score_;

	size_t highest_score_;

	bool is_failed;

	bool is_full;
};

template <typename func_t>
void Board::map(func_t func, position_t _direction) const {
	const auto _i_begin = _direction.y == -1 ? kEdgeLen - 1 : 0;
	const auto _j_begin = _direction.x == -1 ? kEdgeLen - 1 : 0;

	const int _i_limit = _direction.y != -1 ? kEdgeLen : -1;
	const int _j_limit = _direction.x != -1 ? kEdgeLen : -1;

	const int _i_step = _direction.y == -1 ? -1 : 1;
	const int _j_step = _direction.x == -1 ? -1 : 1;

	for (size_t i = _i_begin; i != _i_limit; i += _i_step) {
		for (size_t j = _j_begin; j != _j_limit; j += _j_step) {
			func((*this)[position_t(j, i)], position_t(j, i));
		}
	}
}

class BoardView {

	friend BoardController;

	struct Rect_F {
		float left_;
		float top_;
		float right_;
		float bottom_;

		Rect_F();

		Rect_F(float _left, float _top, float _right, float _bottom);

		explicit Rect_F(const D2D1_RECT_F& _rect);

		D2D1_RECT_F get_self() const;
	};

	struct Size_U {
		uint32_t width_;
		uint32_t height_;

		Size_U();

		Size_U(uint32_t _width, uint32_t _height);

		explicit Size_U(const D2D1_SIZE_U& _size);

		D2D1_SIZE_U get_self() const;
	};

	struct Size_F {
		float width_;
		float height_;

		Size_F();

		Size_F(float _width, float _height);

		explicit Size_F(const D2D1_SIZE_F& _size);

		D2D1_SIZE_F get_self() const;
	};

public:

	BoardView();

	BoardView(HWND _hwnd);

	~BoardView();

public:

	HRESULT init_paint(const HWND& _hwnd);

	HRESULT on_paint(const Board& _board);

	HRESULT failed_paint();

	HRESULT paint_static(const Board& _board);

	HRESULT paint_block_mat(const Board& _board) const;

	HRESULT paint_score_board(
		size_t _score,
		wchar_t* _title,
		Rect_F _position) const;
	HRESULT paint_text(wchar_t* _text, Rect_F _position) const;

	HRESULT paint_score(size_t _score) const;

	HRESULT paint_highest_score(size_t _highest_score) const;

	HRESULT paint_slot(const Board& _board) const;

	HRESULT release_resource();

	BoardView::Size_F transform(
		const Rect_F& _src,
		const Block& _pre_block,
		const Board::position_t& _diff,
		Size_F _step) const;

	void transform_mat(
		const Board::grid_t& _pre_grid,
		Board& _board,
		const std::array<std::array<Board::position_t, 4>, 4>& _diff_mat,
		const Board::direction_t& _direction
	);

	static Rect_F set_rect_margin(
		Rect_F _rect,
		float _left_margin,
		float _top_margin,
		float _right_margin,
		float _bottom_margin);

	Rect_F set_rect_margin(Rect_F _rect, float _margin) const;

private:

	static BoardView::Rect_F calculate_block_pos(Rect_F init_rect, Board::position_t _pos);

	void calculate_center();

	static void set_time_delay_ms(size_t _ms);

private:
	
	com_ptr<ID2D1Factory> board_factory_;
	com_ptr<ID2D1HwndRenderTarget> render_target_;
	com_ptr<ID2D1SolidColorBrush> board_brush_;
	Rect_F rect_;

	com_ptr<IDWriteTextFormat> text_format_;
	com_ptr<IDWriteFactory> write_factory_;

	Button reset_button_{};
};

class BoardController {

public:

	BoardController();

	~BoardController();

public:

	void generate_rand();

	void generate_rand(int _val);

	void move(const Board::position_t& _target, const Board::direction_t& _direction);

	void merge(const Board::position_t& _pos, const Board::direction_t& _direction);

	void release_resource();

	bool failed();

	bool is_operable(const std::string& _arrow_key);

	bool get_failed() const;

	std::vector<Board::position_t> get_valid_slots();

	void update(const Board::direction_t& _direction);

	void reset_board();

	void save_highest_score() const;

	HRESULT handle_key(const std::string& _arrow_key);

	HRESULT init_paint(HWND _hwnd);

	HRESULT on_paint();

private:

	static std::array<std::array<Board::position_t, kEdgeLen>, kEdgeLen> calculate_diff(
		const Board::grid_t& _pre_grid,
		const Board::grid_t& _cur_grid,
		const Board::direction_t& _direction);

	static size_t get_rand(size_t _max);

	static Board::position_t get_normal_vector(const Board::direction_t& _direction);

	bool is_movable(const Board::position_t& _tar, const Board::direction_t& _direction);

	bool is_mergable(const Board::position_t& _tar, const Board::direction_t& _direction);

	static bool is_edge(const Board::position_t& _tar, const Board::direction_t& _direction);

	bool is_same(const Board::position_t& _lhs, const Board::position_t& _rhs);

	static bool is_out_of_range(const Board::position_t& _pos);

	bool is_zero(const Board::position_t& _pos);

private:

	Board board_model_;

	BoardView board_view_;

};

#endif