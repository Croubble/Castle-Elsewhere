#include "World.h"
#include "Animation.h"
#include "Parse.h"



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
		

		//if the player is standing on a staircase tile after a move, apply a teleport to them. Store the final position that our player ends up standing on.
		//todo store the final position as what happens if no teleport.
		int final_level_index = next_square_level;
		int final_position_1d = next_square_position_1d;
		{
			int f = next_state->floor[next_square_position_1d];
			bool standing_on_teleporter = is_staircase(f);
			if (standing_on_teleporter)
			{
				//find the position that our teleporter links to.
				int link_location;
				IntPair link_square_2d;
				int link_square_1d;
				{
					FloorData next_floordata = next_state->floor_data[next_square_position_1d];
					link_location = next_floordata.teleporter_id;
					link_square_2d = next_floordata.teleporter_target_square;
					int link_w = scene->level_state[link_location]->w;
					int link_h = scene->level_state[link_location]->h;
					link_square_1d = f2D(link_square_2d.x, link_square_2d.y, link_w,link_h);
				}
				//remove the player from their current position, teleport them to our new position, and update the new final position for the player.
				{
					next_state->piece[next_square_position_1d] = 0;
					scene->level_state[link_location]->piece[link_square_1d] = Piece::P_PLAYER;
					scene->current_level = link_location;
					final_level_index = link_location;
					final_position_1d = link_square_1d;
				}
			}

		}
		{
			bool next_square_level;
			bool next_square_position_1d;
			//if the player is standing on a "pos level" tile OR the player just entered into a level staircase, initiate a new time_machine.
			{
				//Floor tile_player_standing_on = next_state->floor[next_square_position_1d];
				int tile_player_standing_on = scene->level_state[final_level_index]->floor[final_position_1d];
				bool standing_on_start_tile = tile_player_standing_on == F_START || tile_player_standing_on == F_STAIRCASE_LEVELSTART;
				if (standing_on_start_tile)
				{
					memory_clear(level_memory);
					GameState* next_scene_before_extrude = scene->level_state[scene->current_level];
					GameState* next_scene = gamestate_clone(next_scene_before_extrude, level_memory);
					gamestate_startup(next_scene);
					WorldPlayScene* result = (WorldPlayScene*) memory_alloc(level_memory, sizeof(WorldPlayScene));
					result->time_machine = gamestate_timemachine_create(next_scene, level_memory, 1024);
					result->draw_position = scene->level_position[final_level_index];
					return result;
				}
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
	for (int i = 0; i < num_gamestates;i++)
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
			if (state->floor[z] == Floor::F_START || state->floor[z] == Floor::F_STAIRCASE_LEVELSTART)
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
			char* name = world->level_names[i].name;
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
	for (int i = 0; i < MAX_NUMBER_GAMESTATES; i++)
		for(int j = 0; j < GAME_LEVEL_NAME_MAX_SIZE;j++)
	{
		result->level_names[i].name[j] = '\0';
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
