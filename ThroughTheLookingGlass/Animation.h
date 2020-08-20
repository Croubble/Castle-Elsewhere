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
struct MovementAnimation
{
	int num_to_draw;
	AnimationMoveStartInfo starts;
	AnimationMoveEndInfo ends;
};
*/


const int NUM_CURSE_FLASHES = 3;
struct RealCurseAnimation
{
	glm::vec2* position;	//the position we are drawing the flash at.
	int* draw_value; //the sprite we are drawing. 
	glm::vec4* color_start;	//we animate by taking our object, flashing its color, then taking our object and.. un-flashing the color
	glm::vec4* color_flash;	//the color we flash to 3 times.
};
struct MovementAnimation
{
	int num_elements;
	glm::vec3* start_position;
	glm::vec2* start_offset;
	glm::vec2* end_offset;
	int* sprite_value;
};
struct DrawCurseAnimation
{
	int num_elements;
	bool* flash;
};

struct Animations
{
	GameState* old_state;
	MovementAnimation* maybe_movement_animation;
	DrawCurseAnimation* curse_animation;
};

MovementAnimation* animation_build(GameStateAnimation* animation, GameState* draw_from, IntPair gamestate_start, Memory* clear_on_gamestate_update_memory, float z_pos)
{
	if (animation == NULL)
		return NULL;
	const int num_elements = animation->num_to_draw;
	MovementAnimation* result = (MovementAnimation*) memory_alloc(clear_on_gamestate_update_memory,sizeof(MovementAnimation));
	result->num_elements = animation->num_to_draw * 2;
	result->start_position = (glm::vec3*) memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec3) * num_elements * 2);
	result->sprite_value = (int*)memory_alloc(clear_on_gamestate_update_memory, sizeof(int) * num_elements * 2);
	result->start_offset = (glm::vec2*) memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec2) * num_elements * 2);
	result->end_offset = (glm::vec2*) memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec2) * num_elements * 2);
	//build out the animation.
	{
		PieceMovementAnimation* info = &animation->starts;
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
		PieceMovementAnimation* info = &animation->ends;
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
Animations* animations_build(GameActionJournal* journal, GameState* draw_from, IntPair gamestart, Memory* clear_on_gamestate_update_memory, float z_pos)
{
	memory_clear(clear_on_gamestate_update_memory);
	Animations* result = (Animations*)memory_alloc(clear_on_gamestate_update_memory, sizeof(Animations));
	result->maybe_movement_animation = animation_build(journal->maybe_animation, draw_from, gamestart, clear_on_gamestate_update_memory, z_pos);
	result->old_state = gamestate_clone(journal->old_state, clear_on_gamestate_update_memory);
	result->curse_animation = (DrawCurseAnimation*) memory_alloc(clear_on_gamestate_update_memory, sizeof(DrawCurseAnimation));
	result->curse_animation->num_elements = draw_from->w * draw_from->h;
	result->curse_animation->flash = (bool*) memory_alloc(clear_on_gamestate_update_memory, sizeof(bool) * draw_from->w * draw_from->h);
	for (int i = 0; i < draw_from->w * draw_from->h; i++)
	{
		result->curse_animation->flash[i] = journal->maybe_cursed_animation->flash[i];
	}

	return result;
}