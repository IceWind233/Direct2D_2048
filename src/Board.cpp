#include <array>
#include <random>

#include "Board.h"
#include "Block.h"

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

Board::Board() :
	score(0) {
	map([](Block& _slot, position_t) {
		_slot = Block();
	});

}

Block& Board::operator[](position_t pos) {

	return board[pos.x][pos.y];
}

void Board::generate_rand() {
	auto _pos_arr = get_valid_slots();
	auto rand = generate_rand(_pos_arr.size());
	auto _pos = _pos_arr[rand];

	(*this)[_pos] = Block(2);
}

void Board::move(const position_t& _pos, const direction_t& _direction) {
	while (is_movable(_pos, _direction)) {
		(*this)[_pos + _direction.second] = (*this)[_pos];

		(*this)[_pos].reset(); 
	}
}

void Board::merge(position_t _pos, direction_t _direction) {
	if (!is_edge(_pos, _direction) ||
		(*this)[_pos + _direction.second] == (*this)[_pos]) {

		(*this)[_pos + _direction.second] += (*this)[_pos];

		(*this)[_pos].reset();
	}
}

void Board::update(direction_t _direction) {
	map([&, _direction](Block&, position_t _pos) {
		move(_pos, _direction);
	});
	map([&, _direction](Block&, position_t _pos) {
		merge(_pos, _direction);
	});
	map([&, _direction](Block&, position_t _pos) {
		move(_pos, _direction);
	});
}


//TODO:: Lambda function return or break;
bool Board::failed() {
	if(get_valid_slots().empty()) {
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
	}
	return false;
}

std::vector<Board::position_t> Board::get_valid_slots() {
	std::vector<position_t> _pos_arr;

	map([&_pos_arr](const Block& _slot, position_t _pos) {
		if (static_cast<int>(_slot) == 0) {
			_pos_arr.emplace_back(_pos);
		}
	});

	return _pos_arr;
}

size_t Board::generate_rand(size_t _max) {
	std::random_device _rand;
	std::mt19937 _rng(_rand());
	std::uniform_int_distribution<size_t> _dist(0, _max);

	return _dist(_rng);
}

bool Board::is_movable(const position_t& _tar, direction_t _direction) {
	if (is_edge(_tar, _direction)) {
		return false;
	}
	return static_cast<int>((*this)[_tar + _direction.second]);
}

bool Board::is_edge(const position_t& _tar, direction_t _direction) {
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

bool Board::is_out_of_range(position_t& _pos) {
	if (_pos.x >= kEdgeLen) return true;
	if (_pos.y >= kEdgeLen) return true;
	if (_pos.x < 0) return true;
	if (_pos.y < 0) return true;
	return false;
}


