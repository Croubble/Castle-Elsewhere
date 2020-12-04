#pragma once
#include "GameState.h"
#include "Math.h"
#include "Memory.h"
const float Z_POSITION_STARTING_LAYER = 2;


struct MovementAnimation
{
	int num_elements;
	glm::vec3* position; //where we, in actual space, start drawing from.
	glm::vec2* start_offset;//where the image starts been offset from.
	glm::vec2* end_offset; //where the image ends up been offset from.
	int* sprite_value;
};

struct Animations
{
	GameState* old_state;
	MovementAnimation* maybe_movement_animation;
};

MovementAnimation* movement_animation_build(GameStateAnimation* animation, GameState* draw_from, IntPair gamestate_start, Memory* clear_on_gamestate_update_memory, float z_pos);
Animations* animations_build(GameActionJournal* journal, GameState* draw_from, IntPair gamestart, Memory* clear_on_gamestate_update_memory, float z_pos);