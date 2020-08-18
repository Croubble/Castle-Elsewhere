#include "Math.h"

IntPair math_grid_move_direction(IntPair pos, IntPair move, IntPair dimensions)
{
	int x = pos.x + move.x;
	int y = pos.y + move.y;
	if (x >= dimensions.x)
		x = dimensions.x - 1;
	if (y >= dimensions.y)
		y = dimensions.y - 1;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	return math_intpair_create(x, y);
}
IntPair math_grid_move_direction(int xStart, int yStart, int xMove, int yMove, int w, int h)
{
	int x = xStart + xMove;
	int y = yStart + yMove;
	if (x >= w)
		x = w - 1;
	if (y >= h)
		y = h - 1;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	return math_intpair_create(x, y);
}
IntPair math_grid_move_direction_wrapped(IntPair pos, IntPair move, IntPair dimensions)
{
	int x = pos.x + move.x;
	int y = pos.y + move.y;
	x = x % dimensions.x;
	y = y % dimensions.y;
	if (x < 0)
		x += dimensions.x;
	if (y < 0)
		y += dimensions.y;
	return math_intpair_create(x, y);
}
IntPair math_grid_move_direction_wrapped(int xStart, int yStart, int xMove, int yMove, int w, int h)
{
	int x = xStart + xMove;
	int y = yStart + yMove;
	x = x % w;
	y = y % y;
	if (x < 0)
		x += w;
	if (y < 0)
		y += h;
	return math_intpair_create(x, y);
}
