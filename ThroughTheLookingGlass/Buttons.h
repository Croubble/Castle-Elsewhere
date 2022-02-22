#pragma once
#include "Math.h"

typedef void (*ButtonCallback)(void* input);
struct CollisionButtons
{
	int max_length;
	int current_length;
	AABB* bounds;
	ButtonCallback* callbacks;
};

CollisionButtons* buttons_create_gamespace_collision_buttons(Memory* memory, int max_length)
{
	CollisionButtons* result = mem_alloc<CollisionButtons>(memory, 1);
	result->max_length = max_length;
	result->current_length = 0;
	result->bounds = mem_alloc<AABB>(memory, max_length);
	result->callbacks = mem_alloc<ButtonCallback>(memory, max_length);
	return result;
}

ButtonCallback buttons_maybe_detect_click_callback(CollisionButtons buttons, glm::vec2 click)
{
	int collision = math_click_is_inside_AABB_list(click.x, click.y, buttons.bounds, buttons.current_length);
	if (collision <= 0)
		return NULL;
	return buttons.callbacks[collision];
}
