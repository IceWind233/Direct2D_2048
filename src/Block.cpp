#include "Block.h"

Block::Block():
	value_(0){}

Block::Block(DWORD _value) :
	value_(_value){}

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

void Block::reset() {
	value_ = 0;
}
