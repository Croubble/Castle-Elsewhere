#pragma once
#include "Math.h"
#include "GameState.h"
#include "GenericGamestate.h"
#include "Animation.h"
#include <iostream>

struct WorldScene
{
	int num_levels;
	int current_level;
	char level_names[MAX_NUMBER_GAMESTATES * GAME_LEVEL_NAME_MAX_SIZE];
	GameState* level_state[MAX_NUMBER_GAMESTATES];
	IntPair level_position[MAX_NUMBER_GAMESTATES];
	bool level_solved[MAX_NUMBER_GAMESTATES];
	GamestateTimeMachine* maybe_time_machine;
	Animations* maybe_animation;
};
WorldScene* setup_world_scene(TimeMachineEditor* build_from, Memory* world_scene_memory);

Animations* animation_build_from_world(GameActionJournal* journal, WorldScene* world, Memory* animation_memory)
{
	if (world->maybe_time_machine == NULL)
	{
		std::cout << "calling animation_build_from_world when there is no time machine" << std::endl;
		abort();
	}
	if (journal == NULL)
		return NULL;
	GameState* to_draw = gamestate_timemachine_get_latest_gamestate(world->maybe_time_machine);
	return animations_build(journal, to_draw, world->level_position[world->current_level], animation_memory, Z_POSITION_STARTING_LAYER + LN_PIECE);
}
void world_player_action(WorldScene* scene, Direction action, Memory* level_memory)
{
	//grab some useful information that we will reuse.
	IntPair move = direction_to_intpair(action);
	GameState* current_state = scene->level_state[scene->current_level];
	IntPair current_state_position = scene->level_position[scene->current_level];

	//Find the player.
	IntPair current_player_position;
	int current_player_value;
	{
		int len = current_state->w * current_state->h;
		bool found_player = false;
		for (int i = 0; i < len; i++)
		{
			if (is_player(current_state->layers[LN_PIECE][i]))
			{
				current_player_position = t2D(i, current_state->w, current_state->h);
				current_player_value = current_state->layers[LN_PIECE][i];
				found_player = true;
				break;
			}
		}
		if (!found_player)
		{
			std::cout << "failed to find player in the gamestate it was meant to be in. This is a fatal error, fix it." << std::endl;
			abort();
		}

	}

	//calculate what square we are moving too.
	IntPair next_player_square_position;
	int next_square_level;
	{
		IntPair tentative_next = math_intpair_add(move, current_player_position);
		bool inside_level = math_within_grid(tentative_next.x, tentative_next.y, current_state->w, current_state->h);
		if (inside_level)
		{
			next_player_square_position = tentative_next;
			next_square_level = scene->current_level;
		}
		else
		{
			//test to see if there are any levels who touch this level.
			IntPair world_position = math_intpair_add(tentative_next, current_state_position);
			//subtract the offset between the two positions, then see if that position is on the world.
			bool found_move = false;
			for (int i = 0; i < scene->num_levels; i++)
			{
				IntPair grid_location = math_intpair_sub(world_position, scene->level_position[i]);
				if (math_within_grid(grid_location.x, grid_location.y, scene->level_state[i]->w, scene->level_state[i]->h))
				{
					next_player_square_position = grid_location;
					next_square_level = i;
					found_move = true;
					break;
				}
			}
			if (!found_move)
			{
				//if there isn't anywhere to move, there is no more work to be done, just leave the function and return.
				return;
			}
		}
	}

	//check to see that the square we are moving to is empty, and if its not, just leave the function.
	GameState* next_state = scene->level_state[next_square_level];
	{
		int next_square_position_1d = f2D(next_player_square_position.x, next_player_square_position.y, next_state->w, next_state->h);
		if (next_state->layers[LN_PIECE][next_square_position_1d] != P_NONE)
		{
			//we can't make the action, its invalid. Just reeturn.
			return;
		}
	}

	//were ready! Apply the move.
	{
		//remove the player from the current position.
		{
			int current_state_position_1d = f2D(current_player_position.x, current_player_position.y, current_state->w, current_state->h);
			current_state->layers[LN_PIECE][current_state_position_1d] = P_NONE;
		}
		//add the player to the next position
		int next_square_position_1d = f2D(next_player_square_position.x, next_player_square_position.y, next_state->w, next_state->h);
		{
			next_state->layers[LN_PIECE][next_square_position_1d] = current_player_value;
			scene->current_level = next_square_level;
		}
		//if the player is standing on a "pos level" tile, initiate a new time_machine.
		{
			bool standing_on_start_tile = next_state->layers[LN_FLOOR][next_square_position_1d] == F_START;
			if (standing_on_start_tile)
			{
				//TODO.
				memory_clear(level_memory);
				GameState* next_scene_before_extrude = scene->level_state[scene->current_level];
				GameState* next_scene = gamestate_clone(next_scene_before_extrude,level_memory);
				gamestate_extrude_lurking_walls(next_scene);
				scene->maybe_time_machine = gamestate_timemachine_create(next_scene, level_memory, 1024);
			}
		}
	}
}
WorldScene* setup_world_scene(TimeMachineEditor* build_from, Memory* world_scene_memory)
{
	WorldScene* result = (WorldScene*)memory_alloc(world_scene_memory, sizeof(WorldScene));
	const int num_gamestates = build_from->current_number_of_gamestates;
	result->num_levels = num_gamestates;
	result->maybe_time_machine = NULL;
	result->maybe_animation = NULL;
	for (int i = 0; i < num_gamestates; i++)
	{
		result->level_position[i] = build_from->gamestates_positions[i];
	}
	for (int i = 0; i < num_gamestates; i++)
	{
		result->level_solved[i] = false;
	}
	for (int i = 0; i < num_gamestates; i++)
	{
		result->level_state[i] = gamestate_clone(build_from->gamestates[i], world_scene_memory);
	}
	for (int i = 0; i < num_gamestates * GAME_LEVEL_NAME_MAX_SIZE; i++)
	{
		result->level_names[i] = build_from->names[i];
	}
	//find a player in our starter level.
	{
		bool done_finding_player = false;
		for (int i = 0; i < num_gamestates; i++)
		{
			int len = result->level_state[i]->w * result->level_state[i]->h;
			for(int z = 0; z < len;z++)
				if (result->level_state[i]->layers[LN_PIECE][z] == P_PLAYER)
				{
					result->current_level = i;
					done_finding_player = true;
					break;
				}
			if (done_finding_player)
				break;
		}
		if (!done_finding_player)
		{
			std::cout << "You tried to instantiate a world that didn't contain a player, so we can't pos the world. For now, this is a crash." << std::endl;
			abort();
		}
	}
	return result;
}
