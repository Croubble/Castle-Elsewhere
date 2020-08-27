#pragma once
#include "GameState.h"
#include "Math.h"
#include "Memory.h"
const float Z_POSITION_STARTING_LAYER = 2;

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

MovementAnimation* animation_build(GameStateAnimation* animation, GameState* draw_from, IntPair gamestate_start, Memory* clear_on_gamestate_update_memory, float z_pos);
Animations* animations_build(GameActionJournal* journal, GameState* draw_from, IntPair gamestart, Memory* clear_on_gamestate_update_memory, float z_pos);