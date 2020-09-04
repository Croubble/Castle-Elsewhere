#include "Math.h"


void math_AABB_break_into_borders(AABB box, AABB* output, float border_size)
{
	//add down
	output[0] = math_AABB_create(box.x, box.y, box.w, border_size);
	//add up
	output[1] = math_AABB_create(box.x, box.y + box.h - border_size, box.w, border_size);
	//add left
	output[2] = math_AABB_create(box.x,box.y,border_size,box.h);
	//add right
	output[3] = math_AABB_create(box.x + box.w - border_size,box.y,border_size, box.h);
}
AABB math_AABB_create(float x, float y, float width, float height)
{
	AABB result;
	result.x = x;
	result.y = y;
	result.w = width;
	result.h = height;
	return result;
}
AABB math_AABB_create_int(int x, int y, int width, int height)
{
	AABB result;
	result.x = (float) x;
	result.y = (float) y;
	result.w = (float) width;
	result.h = (float) height;
	return result;
}

bool math_AABB_is_colliding(AABB test, AABB* against_array, int length)
{
	for (int i = 0; i < length; i++)
	{
		AABB against = against_array[i];
		if (test.x < against.x + against.w &&
			test.x + test.w > against.x &&
			test.y < against.y + against.h &&
			test.y + test.h > against.y)
		{
			return true;
		}
	}
	return false;
}
bool math_AABB_is_colliding(AABB test, AABB* against_array, int length, int* collision_index_out)
{
	for (int i = 0; i < length; i++)
	{
		AABB against = against_array[i];
		if (test.x < against.x + against.w &&
			test.x + test.w > against.x &&
			test.y < against.y + against.h &&
			test.y + test.h > against.y)
		{
			return i;
		}
	}
	return false;
}
bool math_AABB_is_colliding(AABB test, AABB* against_array, int length, int* collision_indexes_out, int max_collisions_to_find)
{
	int current_number_of_collisions = 0;
	for (int i = 0; i < length; i++)
	{
		AABB against = against_array[i];
		if (test.x < against.x + against.w &&
			test.x + test.w > against.x &&
			test.y < against.y + against.h &&
			test.y + test.h > against.y)
		{
			collision_indexes_out[current_number_of_collisions] = i;
			current_number_of_collisions++;
			if (current_number_of_collisions >= max_collisions_to_find)
				return true;
		}
	}
	return false;
}

int math_click_is_inside_AABB_list(float clickX, float clickY, AABB* boxes, int total_boxes)
{
	for (int i = 0; i < total_boxes; i++)
	{
		if (math_click_is_inside_AABB(clickX, clickY, boxes[i]))
			return i;
	}
	return -1;
}
