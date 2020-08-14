#pragma once

#include "Math.h"

bool* math_grid_walk_adjacent(Memory* result_memory, Memory* temp_memory, int* input_grid, int start_x, int start_y, int w, int h)
{
	return nullptr;
}
bool* math_grid_walk_adjacent_wrapped(Memory* result_memory, Memory* temp_memory, int* input_grid, int start_x, int start_y, int w, int h)
{	
	/*
	//setup result.
	IntPair dimensions = math_intpair_create(w, h);
	int grid_length = w * h;

	bool* result = (bool*)memory_alloc(result_memory, sizeof(char) * grid_length);
	for (int i = 0; i < grid_length; i++)
	{
		result[i] = false;
	}

	//instantiate algorithm in starting state.
	IntPair* position_to_examine = (IntPair*)memory_alloc(temp_memory, sizeof(IntPair) * grid_length);
	bool* position_is_examined = (bool*)memory_alloc(temp_memory, sizeof(bool) * grid_length);
	int current_position = 0;
	int position_to_examine_length = 1;

	//input first element to algorithm.
	position_to_examine[0] = math_intpair_create(start_x, start_y);
	position_is_examined[a2D(start_x, start_y, w, h)] = true;

	//MAIN LOOP
	for (int i = current_position; i < position_to_examine_length && i < grid_length; i++)
	{
		IntPair examined_position = position_to_examine[i];
		position_is_examined[a2D(examined_position, w, h)] = true;
		for (int s = 0; s < S_COUNT; s++)
		{
			IntPair side_intpair = math_side_to_direction((Side) s);
			IntPair adjacent = math_intpair_add(side_intpair, examined_position);
			if (
				!position_is_examined[a2D(adjacent, w, h)])
			{

			}
		}
		//check each adjacent position.
		//does it match our criteria?
		//has it yet to be added to the success list?
			//add it to the success list, and add it to be examined.
	}
	*/
	return nullptr;
}