#pragma once
#include "Math.h"

typedef void (*Callback_function)();
struct CollisionButtons
{
	int max_length;
	int current_length;
	AABB* bounds;
	Callback_function* callbacks;
};

CollisionButtons* buttons_create_gamespace_collision_buttons(Memory* memory, int max_length)
{
	CollisionButtons* result = mem_alloc<CollisionButtons>(memory, 1);
	result->max_length = max_length;
	result->current_length = 0;
	result->bounds = mem_alloc<AABB>(memory, max_length);
	result->callbacks = mem_alloc<Callback_function>(memory, max_length);
	return result;
}

Callback_function buttons_maybe_detect_click_callback(CollisionButtons buttons, glm::vec2 click)
{
	int collision = math_click_is_inside_AABB_list(click.x, click.y, buttons.bounds, buttons.current_length);
	if (collision <= 0)
		return NULL;
	return buttons.callbacks[collision];
}
