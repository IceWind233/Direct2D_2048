#ifndef BLOCK_H
#define BLOCK_H

#include <Windows.h>

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

private:
	DWORD value_;
};

#endif