#pragma once

#include "Math.h"


IntPair math_side_to_direction(Side s)
{
	if (s == S_RIGHT)
		return math_intpair_create(1, 0);
	if (s == S_DOWN)
		return math_intpair_create(0, -1);
	if (s == S_LEFT)
		return math_intpair_create(-1, 0);
	if (s == S_UP)
		return math_intpair_create(0, 1);
	return math_intpair_create(0, 0);
}
Side math_direction_to_side(IntPair pair)
{
	if (pair.x == -1)
		return S_LEFT;
	if (pair.x == 1)
		return S_RIGHT;
	if (pair.y == 1)
		return S_UP;
	if (pair.y == -1)
		return S_DOWN;
}
Side math_side_opposite(Side s)
{
	//we could just do if(s == Side.Right) Side.Left; ... but instead, we are just going to do maths and save wasting int stuff.
	int i = (int)s;
	int next = (i + 2) % 4;
	return (Side) next;
}
