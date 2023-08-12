#ifndef BOARD_H
#define BOARD_H

#pragma once

#include <vector>
#include <map>
#include <array>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#include "Block.h"

constexpr size_t kEdgeLen = 4;
constexpr FLOAT kRadius = 10.f;
constexpr FLOAT kBlockFontSize = 20.f;
const auto kFontFamily = L"JetBrains Mono Medium";
const D2D1::ColorF kBoardColor = D2D1::ColorF(.73f, .68f, .63f);
const D2D1::ColorF kBackGroundColor = D2D1::ColorF(.98f, .97f, .93f);

class Board {

private:

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

	std::map<std::string, position_t> direction = {
		{"kUp",		position_t(0, -1)},
		{"kRight",	position_t(1, 0)},
		{"kDown",	position_t(0, 1)},
		{"kLeft",	position_t(-1, 0)},
	};

public:

	Board();

	~Board();

public:

	Block& operator[](position_t pos);

	Block operator[](position_t pos) const;

public:

	void generate_rand();

	void move(const position_t& _target, const direction_t& _direction);

	void merge(const position_t& _pos, const direction_t& _direction);

	void update(const direction_t& _direction, HWND _hwnd);

	void calculate_center();

	D2D1_SIZE_F transform(
		const D2D1_RECT_F& _src,
		const Block& _pre_block, const position_t& _diff, D2D1_SIZE_F _step) const;

	void transform_mat(
		const grid_t& _pre_grid,
		const std::array<std::array<position_t, 4>, 4>& _diff_mat,
		HWND _hwnd, const direction_t& _direction
	);

	bool failed();

	bool get_failed();

	bool is_operable(const std::string& _arrow_key);

	HRESULT init_paint(HWND _hwnd);

	HRESULT on_paint();

	HRESULT failed_paint();

	HRESULT paint_block_mat();

	HRESULT paint_score();

	HRESULT paint_slot();

	HRESULT handle_key(std::string _arrow_key, HWND _hwnd);

	HRESULT release_resource();

	std::vector<position_t> get_valid_slots();

private:

	template <typename func_t>
	void map(func_t func, position_t _direction = position_t(0, 1));

	template <typename func_t>
	void map_all_direction(func_t func);

	D2D1_RECT_F calculate_block_pos(D2D1_RECT_F init_rect, position_t _pos);

	std::array<std::array<position_t, kEdgeLen>, kEdgeLen> calculate_diff(
		const grid_t& _pre_grid,
		const grid_t& _cur_grid,
		const direction_t& _direction) const;

	size_t get_rand(size_t _max);

	position_t get_normal_vector(const direction_t& _direction) const;

	bool is_movable(const position_t& _tar, const direction_t& _direction);

	bool is_mergable(const position_t& _tar, const direction_t& _direction);

	bool is_edge(const position_t& _tar, const direction_t& _direction);

	bool is_same(const position_t& _lhs, const position_t& _rhs);

	bool is_out_of_range(const position_t& _pos);

	bool is_zero(const position_t& _pos);

private:

	grid_t board_;

	size_t score_;

	bool is_failed;

	bool is_full;

	ID2D1Factory* board_factory_{};
	ID2D1HwndRenderTarget* render_target_{};
	ID2D1SolidColorBrush* board_brush_{};
	ID2D1SolidColorBrush* trans_brush_{};
	D2D1_RECT_F rect_;

	IDWriteTextFormat* text_format_{};
	IDWriteFactory* write_factory_{};

};



template <typename func_t>
void Board::map(func_t func, position_t _direction) {

	auto _i_begin = _direction.y == -1 ? kEdgeLen - 1: 0;
	auto _j_begin = _direction.x == -1 ? kEdgeLen - 1: 0;

	int _i_limit = _direction.y != -1 ? kEdgeLen : -1;
	int _j_limit = _direction.x != -1 ? kEdgeLen : -1;

	int _i_step = _direction.y == -1 ? -1 : 1;
	int _j_step = _direction.x == -1 ? -1 : 1;

	for (size_t i = _i_begin; i != _i_limit; i += _i_step) {
		for (size_t j = _j_begin; j != _j_limit; j += _j_step) {
			func((*this)[position_t(j, i)], position_t(j, i));
		}
	}
}

template <typename func_t>
void Board::map_all_direction(func_t func) {
	for (auto _direction : direction) {
		func(_direction);
	}
}

#endif