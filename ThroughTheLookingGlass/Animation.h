#pragma once
#include "GameState.h"
#include "Math.h"
#include "Memory.h"
const float Z_POSITION_STARTING_LAYER = 2;


struct MovementAnimation
{
	int num_elements;
	glm::vec3* start_position;
	glm::vec2* start_offset;
	glm::vec2* end_offset;
	int* sprite_value;
};

struct Animations
{
	GameState* old_state;
	MovementAnimation* maybe_movement_animation;
};

MovementAnimation* animation_build(GameStateAnimation* animation, GameState* draw_from, IntPair gamestate_start, Memory* clear_on_gamestate_update_memory, float z_pos);
Animations* animations_build(GameActionJournal* journal, GameState* draw_from, IntPair gamestart, Memory* clear_on_gamestate_update_memory, float z_pos);