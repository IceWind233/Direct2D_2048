#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <map>

#include "Block.h"

/*
 *
 *Board(); init Board
 *|
 *generate; Block(2)
 *|
 *wait key ;
 *|
 *update(); move -> merge -> move
 *|
 *if(get_valid_slot == 0) goto(2);
 *|
 *if(!failed) goto(2);
 *|
 *Failed print score;
 */

constexpr size_t kEdgeLen = 4;

class Board {

private:

	struct position_t;

	typedef std::pair<std::string, position_t> direction_t;
	typedef std::array<std::array<Block, kEdgeLen>, kEdgeLen> grid_t;

	struct position_t {
		size_t x;
		size_t y;

		position_t(size_t _x, size_t _y)
			: x(_x), y(_y){ }

		position_t(const position_t& _other) = default;

		position_t(position_t&& _ref) = default;

		position_t& operator+=(const position_t& _rhs);

		position_t& operator-() const;

		position_t& operator-=(const position_t& _rhs);

		friend position_t& operator+(const position_t& lhs, const position_t& _rhs) {
			position_t tmp(lhs);

			return tmp += _rhs;
		}

		friend position_t& operator-(const position_t& lhs, const position_t& _rhs) {
			position_t tmp(lhs);

			return tmp -= _rhs;
		}

		friend position_t& operator+(const position_t& _lhs, const direction_t& _rhs) {
			position_t tmp(_lhs);
			tmp.x += _rhs.second.x;
			tmp.y += _rhs.second.y;

			return tmp;
		}

		friend position_t& operator-(const position_t& _lhs, const direction_t& _rhs) {
			position_t tmp(_lhs);
			tmp.x -= _rhs.second.x;
			tmp.y -= _rhs.second.y;

			return tmp;
		}

	};

	const std::map<std::string, position_t> direction = {
		{"kUp",		position_t(0, -1)},
		{"kRight",	position_t(1, 0)},
		{"kDown",	position_t(0, 1)},
		{"kLeft",	position_t(-1, 0)},
	};

public:

	Board();

	~Board() = default;

public:

	Block& operator[](position_t pos);

public:

	void generate_rand();

	void move(const position_t& _target, const direction_t& _direction);

	void merge(position_t _pos, direction_t _direction);

	void update(direction_t _direction);

	bool failed();

	std::vector<position_t> get_valid_slots();

private:

	template <typename func_t>
	void map(func_t func);

	template <typename func_t>
	void map_all_direction(func_t func);

	template <typename func_t>
	void switch_direction(func_t _func, direction_t _direction);

	size_t generate_rand(size_t _max);

	bool is_movable(const position_t& _tar, direction_t _direction);

	bool is_edge(const position_t& _tar, direction_t _direction);

	bool is_same(const position_t& _lhs, const position_t& _rhs);

	bool is_out_of_range(position_t& _pos);

private:

	grid_t board;

	size_t score;

};

template <typename func_t>
void Board::map(func_t func) {
	size_t i = 0;
	for (auto& _row : board) {
		size_t j = 0;
		for (auto& _slot : _row) {
			func(_slot, position_t(j, i));
			++j;
		}
		++i;
	}
}

template <typename func_t>
void Board::map_all_direction(func_t func) {
	for (auto _direction : direction) {
		func(_direction);
	}
}


// TODO:: try to use lambda
template <typename func_t>
void Board::switch_direction(func_t _func, direction_t _direction) {
	switch (_direction) {
	case kUp: {
		_func();
		return;
	}
	case kRight: {
		_func();
		return;
	}
	case kDown: {
		_func();
		return;
	}
	case kLeft: {
		_func();
		return;
	}
	}
}


#endif
