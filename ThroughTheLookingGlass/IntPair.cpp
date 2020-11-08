#include "Math.h"

Direction direction_reverse(Direction dir)
{
	if (dir == Direction::U)
		return Direction::D;
	if (dir == Direction::L)
		return Direction::R;
	if (dir == Direction::D)
		return Direction::U;
	if (dir == Direction::R)
		return Direction::L;
	return dir;
}

Direction direction_rotate_clockwise(Direction dir) {
	if (dir >= NO_DIRECTION)
		return dir;
	return (Direction)(((int)dir + 1) % 4);
}

Direction direction_rotate_anti(Direction dir)
{
	if (dir == Direction::U)
		return Direction::L;
	if (dir == Direction::L)
		return Direction::D;
	if (dir == Direction::D)
		return Direction::R;
	if (dir == Direction::R)
		return Direction::U;
	return dir;
}

IntPair math_intpair_create(int x, int y)
{
	IntPair result;
	result.x = x;
	result.y = y;
	return result;
}
IntPair math_intpair_add(IntPair left, IntPair right)
{
	IntPair result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	return result;
}
IntPair math_intpair_sub(IntPair left, IntPair right)
{
	IntPair result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	return result;
}

bool math_intpair_eq(IntPair left, IntPair right)
{
	return left.x == right.x && left.y == right.y;
}
