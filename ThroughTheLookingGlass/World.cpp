#include "World.h"
#include "Animation.h"
#include "Parse.h"


Animations* animation_build_from_world(GameActionJournal* journal, GamestateTimeMachine* maybe_time_machine, IntPair draw_position, Memory* animation_memory)
{
	if (maybe_time_machine == NULL)
	{
		std::cout << "calling animation_build_from_world when there is no time machine" << std::endl;
		abort();
	}
	if (journal == NULL)
		return NULL;
	GameState* to_draw = gamestate_timemachine_get_latest_gamestate(maybe_time_machine);
	return animations_build(journal, to_draw, draw_position, animation_memory, Z_POSITION_STARTING_LAYER + LN_PIECE);
}
WorldPlayScene* world_player_action(WorldScene* scene, Direction action, Memory* level_memory)
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
			if (is_player(current_state->piece[i]))
			{
				current_player_position = t2D(i, current_state->w, current_state->h);
				current_player_value = current_state->piece[i];
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
				return NULL;
			}
		}
	}

	//check to see that the square we are moving to is empty, and if its not, just leave the function.
	GameState* next_state = scene->level_state[next_square_level];
	{
		int next_square_position_1d = f2D(next_player_square_position.x, next_player_square_position.y, next_state->w, next_state->h);
		if (next_state->piece[next_square_position_1d] != P_NONE)
		{
			//we can't make the action, its invalid. Just reeturn.
			return NULL;
		}
	}

	//were ready! Apply the move.
	{
		//remove the player from the current position.
		{
			int current_state_position_1d = f2D(current_player_position.x, current_player_position.y, current_state->w, current_state->h);
			current_state->piece[current_state_position_1d] = P_NONE;
		}
		//add the player to the next position
		int next_square_position_1d = f2D(next_player_square_position.x, next_player_square_position.y, next_state->w, next_state->h);
		{
			next_state->piece[next_square_position_1d] = current_player_value;
			scene->current_level = next_square_level;
		}
		//if the player is standing on a "pos level" tile, initiate a new time_machine.
		{
			bool standing_on_start_tile = next_state->floor[next_square_position_1d] == F_START;
			if (standing_on_start_tile)
			{
				//TODO.
				memory_clear(level_memory);
				GameState* next_scene_before_extrude = scene->level_state[scene->current_level];
				GameState* next_scene = gamestate_clone(next_scene_before_extrude, level_memory);
				gamestate_extrude_lurking_walls(next_scene);
				WorldPlayScene* result = (WorldPlayScene*) memory_alloc(level_memory, sizeof(WorldPlayScene));
				//result->draw_position;
				//scene->maybe_time_machine = gamestate_timemachine_create(next_scene, level_memory, 1024);
				result->time_machine = gamestate_timemachine_create(next_scene, level_memory, 1024);
				result->draw_position = scene->level_position[next_square_level];
				result->maybe_animation = NULL;
				return result;
			}
		}
	}
	return NULL;
}
WorldScene* setup_world_scene_continue(WorldScene* scene, SCENE_TYPE go_to_on_backspace)
{
	scene->go_to_on_backspace = go_to_on_backspace;
	return scene;
}
WorldScene* setup_world_scene(TimeMachineEditor* build_from, Memory* world_scene_memory, SCENE_TYPE go_to_on_backspace)
{
	WorldScene* result = (WorldScene*)memory_alloc(world_scene_memory, sizeof(WorldScene));
	result->go_to_on_backspace = go_to_on_backspace;

	const int num_gamestates = build_from->current_number_of_gamestates;
	result->num_levels = num_gamestates;
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
			for (int z = 0; z < len; z++)
				if (result->level_state[i]->piece[z] == P_PLAYER)
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
bool any_levels_left_active(WorldScene* to_check)
{
	for (int i = 0; i < to_check->num_levels; i++)
	{
		GameState* state = to_check->level_state[i];
		int w = state->w;
		int h = state->h;
		int len = w * h;
		for (int z = 0; z < len; z++)
		{
			if (state->floor[z] == Floor::F_START)
				return true;
		}
	}
	return false;
}


std::string world_serialize(WorldScene* world, Memory* scope, Memory* temp_memory)
{
	//serialize num levels.
	const int max_length = 10000;
	int output_consumed = 0;
	char* output = (char*)memory_alloc(temp_memory, sizeof(char) * max_length);
	const int num_gamestates = world->num_levels;

	//parse num gamestates.
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "num_gamestates:%d;\n", num_gamestates);
	}
	//parse current level.
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "current_level:%d;\n", world->current_level);
	}

	//positions:
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "positions:");
		for (int i = 0; i < num_gamestates; i++)
			output_consumed += sprintf_s(output + output_consumed, max_length, "%d,%d,", world->level_position[i].x, world->level_position[i].y);
		output_consumed += sprintf_s(output + output_consumed, max_length, ";\n");
	}
	
	//serialize all gamestate elements.
	parse_serialize_gamestate_layers(output, &output_consumed, max_length, num_gamestates, world->level_state);
	//serialize the gamestates names.
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "names:");
		for (int i = 0; i < num_gamestates; i++)
		{
			char* name = &world->level_names[i * GAME_LEVEL_NAME_MAX_SIZE];
			output_consumed += sprintf_s(output + output_consumed, max_length, "%s,", name);
		}
		output_consumed += sprintf_s(output + output_consumed, max_length, ";\n");
	}

	//ttttt
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "solved:");
		for (int i = 0; i < num_gamestates; i++)
		{
			output_consumed += sprintf_s(output + output_consumed, max_length, "%d,", world->level_solved[i]);
		}
		output_consumed += sprintf_s(output + output_consumed, max_length, ";\n");
	}
	return std::string(output);
}
WorldScene* world_deserialize(std::string world_string, Memory* scope, Memory* temp_memory)
{	
	//strip all whitespace from the input.
	{
		// end_pos = std::remove(input_string.begin(), input_string.end(), ' ');
		//input_string.erase(end_pos, input_string.end());
		std::string::iterator end_pos = std::remove(world_string.begin(), world_string.end(), '\n');
		world_string.erase(end_pos, world_string.end());
	}

	WorldScene* result = (WorldScene*)memory_alloc(scope, sizeof(WorldScene));
	for (int i = 0; i < MAX_NUMBER_GAMESTATES; i++)
	{
		result->level_state[i] = NULL;
	}
	for (int i = 0; i < GAME_LEVEL_NAME_MAX_SIZE * MAX_NUMBER_GAMESTATES; i++)
	{
		result->level_names[i] = '\0';
	}
	//get input
	char* input = &(world_string[0]);
	Tokenizer tokenizer;
	tokenizer.at = input;

	//parse number gamestates.
	int* maybe_num_gamestates = try_parse_num_gamestates(&tokenizer, temp_memory);
	if (!maybe_num_gamestates)
		crash_err("failed to pass num gamestates");
	result->num_levels = *maybe_num_gamestates;
	//parse current level.
	int* current_level = try_parse_current_level(&tokenizer, temp_memory);
	if (!current_level)
		crash_err("failed to pass current level");
	result->current_level = *current_level;
	//parse positions:
	{
		bool success = try_parse_positions(&tokenizer, result->level_position, temp_memory);
		if (!success)
			crash_err("failed to parse positions when loading a saved game");
	}
	//parse gamestate elements.
	{
		for(int i = 0; i < LN_COUNT;i++)
			bool success = try_parse_layer(&tokenizer, result->level_state, scope, temp_memory);
	}
	//parse gamestate names.
bool parsed_names = try_parse_names(&tokenizer, result->level_names, scope, temp_memory);
	//parse solved.
bool parsed_solved = try_parse_bools(&tokenizer, result->level_solved, scope, temp_memory);
	return result;
}
