#pragma once
#include "GameState.h"
#include "Math.h"
#include "Memory.h"
const float Z_POSITION_STARTING_LAYER = 2;
const int MAX_CRATE_ANIMATIONS = 4;

template<class T, int L> 
struct TimeAnimation
{
	float time[L];
	T elements[L];
};
struct MovementAnimation
{
	int num_elements;
	glm::vec3* position; //where we, in actual space, start drawing from.
	glm::vec2* start_offset;//where the image starts been offset from.
	glm::vec2* end_offset; //where the image ends up been offset from.
	//glm::vec4* window; <- this value is implicitly set at 0,0,1,1, and describes a window at which point 
	int* sprite_value;
};

struct SymbolMovementAnimation
{
	int num_elements;
	glm::vec3* local_position_start; //where the symbol is locally, and where it ends locally.
	glm::vec3* local_position_end;
	glm::vec3* local_scale_start;
	glm::vec3* local_scale_end;
	glm::vec3* global_position;
	glm::vec2* start_offset; //where the image starts been offset from.
	glm::vec2* end_offset; 
	int* sprite_value; //

	//glm::vec4* window; <- this value is implicitly set at 0,0,1,1, and describes a window. If the image that would be drawn is outside this window, 
};

struct Animations
{
	GameState* old_state;
	MovementAnimation* maybe_movement_animation;
};

SymbolMovementAnimation* symbol_movement_animation_build(GameStateAnimation* animation, GameState* draw_from, IntPair gamestate_start, Memory* clear_on_gamestate_update_memory, float z_pos)
{
	SymbolMovementAnimation* result = (SymbolMovementAnimation*) memory_alloc(clear_on_gamestate_update_memory, sizeof(SymbolMovementAnimation));
	int length = (animation->symbol_start.len + animation->symbol_end.len) * CP_COUNT;
	result->local_position_start = (glm::vec3*)memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec3) * length);//where the symbol is locally, and where it ends locally.
	result->local_position_end = (glm::vec3*)memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec3)* length);
	result->local_scale_start = (glm::vec3*)memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec3)* length);
	result->local_scale_end = (glm::vec3*)memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec3)* length);
	result->global_position = (glm::vec3*)memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec3)* length);
	result->start_offset = (glm::vec2*)memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec2)* length); //where the image starts been offset from.
	result->end_offset = (glm::vec2*)memory_alloc(clear_on_gamestate_update_memory, sizeof(glm::vec2)* length); 
	result->sprite_value = (int*)memory_alloc(clear_on_gamestate_update_memory, sizeof(int)* length); //
	
	int count = 0;
	for (int i = 0; i < animation->symbol_start.len; i++)
	{
		int num_symbols = 0;
		for (int j = 0; j < CP_COUNT; j++)
		{
			num_symbols += animation->symbol_start.start_powers->powers[j];
		}
		int symbol_width = ceil(sqrt(num_symbols));
		int num_drawn = 0;
		for (int j = 0; j < CP_COUNT; j++)
		{
			glm::vec3 draw_position = glm::vec3(num_drawn % symbol_width, num_drawn / symbol_width, 5);
			glm::vec3 scale = glm::vec3(1.0 / (float)symbol_width , 1.0 / (float)symbol_width , 1);
			scale.x -= 0.10f;
			scale.y -= 0.10f;
			draw_position.x /= (float)symbol_width;
			draw_position.y /= (float)symbol_width;
			draw_position.x += 0.05f;
			draw_position.y += 0.05f;
			num_drawn++;
			result->global_position[count] = glm::vec3(animation->symbol_start.global_position[i], animation->symbol_start.global_position[i].y,5);
			result->local_position_start[count] = draw_position;
			result->local_position_end[count]
		}

	}
	for (int i = 0; i < animation->symbol_end.len; i++)
	{

	}
}
MovementAnimation* movement_animation_build(GameStateAnimation* animation, GameState* draw_from, IntPair gamestate_start, Memory* clear_on_gamestate_update_memory, float z_pos);
Animations* animations_build(GameActionJournal* journal, GameState* draw_from, IntPair gamestart, Memory* clear_on_gamestate_update_memory, float z_pos);