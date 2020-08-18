#pragma once
#include "GameState.h"
#include "World.h"


const float Z_POSITION_STARTING_LAYER = 2;
/*
struct AnimationMoveStartInfo
{
	IntPair* pos;
	Direction* to_move;
	int* start_value;
};
struct AnimationMoveEndInfo
{
	IntPair* end;
	Direction* end_direction_we_enter_from;
	int* end_value;
};
struct Animation
{
	int num_to_draw;
	AnimationMoveStartInfo starts;
	AnimationMoveEndInfo ends;
};
*/

struct Animation
{
	int num_elements;
	glm::vec3* start_position;
	glm::vec2* start_offset;
	glm::vec2* end_offset;
	int* sprite_value;
};

Animation* animation_build(GameStateAnimation* animation, GameState* draw_from, IntPair gamestate_start, Memory* clear_on_gamestate_update_memory, float z_pos)
{
	memory_clear(clear_on_gamestate_update_memory);
	const int num_elements = animation->num_to_draw;
	Animation* result = (Animation*) memory_alloc(clear_on_gamestate_update_memory,sizeof(Animation));
	result->num_elements = animation->num_to_draw * 2;
	result->start_position = (glm::vec3*) memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec3) * num_elements * 2);
	result->sprite_value = (int*)memory_alloc(clear_on_gamestate_update_memory, sizeof(int) * num_elements * 2);
	result->start_offset = (glm::vec2*) memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec2) * num_elements * 2);
	result->end_offset = (glm::vec2*) memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec2) * num_elements * 2);
	//build out the animation.
	{
		AnimationMoveInfo* info = &animation->starts;
		for (int i = 0; i < num_elements; i++)
		{
			result->start_position[i] = glm::vec3(info->pos[i].x + gamestate_start.x, info->pos[i].y + gamestate_start.y, z_pos);
			IntPair move = direction_to_intpair(info->to_move[i]);
			if (move.x != 0 || move.y != 0)
			{
				int z = 0;
				z += 3;
				z -= 2;
			}
			result->sprite_value[i] = info->start_value[i];
			result->start_offset[i] = glm::vec2(0, 0);
			result->end_offset[i] = glm::vec2(move.x, move.y);
		}
	}
	{
		AnimationMoveInfo* info = &animation->ends;
		for (int i = 0; i < num_elements; i++)
		{
			IntPair move = direction_to_intpair(info->to_move[i]);
			if (move.x != 0 || move.y != 0)
			{
				int z = 0;
				z += 3;
				z -= 2;
			}
			result->start_position[i + num_elements] = glm::vec3(info->pos[i].x + gamestate_start.x,info->pos[i].y + gamestate_start.y, z_pos);
			result->sprite_value[i + num_elements] = info->start_value[i];
			result->start_offset[i + num_elements] = glm::vec2(-move.x, -move.y);
			result->end_offset[i + num_elements] = glm::vec2(0, 0);
		}
	}
	return result;
}

