#pragma once

#include "Math.h"

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
