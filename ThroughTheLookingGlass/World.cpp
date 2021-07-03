#include "World.h"
#include "Animation.h"
#include "Parse.h"


void world_try_reversing_staircase(WorldScene* scene)
{
	if (scene->staircase_we_entered_level_from.level_index >= 0)
	{
		scene->last_action_was_teleport = true;
		WorldPosition current_pos = world_maybe_find_player(scene);
		WorldPosition revert_pos = scene->staircase_we_entered_level_from;
		if (current_pos.level_index == -1)
			crash_err("uh ohhh, we tried to find a player but couldn't find one, that shouldn't have happened");
		int player_val = scene->world_state.level_state[current_pos.level_index]->piece[current_pos.level_position_1d];
		scene->world_state.level_state[revert_pos.level_index]->floor[revert_pos.level_position_1d] = F_STAIRCASE_SOLVED;
		
		//delete the player from the current position, and put them on our old position. TODO: do this when we backspace as well.
		scene->world_state.level_state[current_pos.level_index]->piece[current_pos.level_position_1d] = P_NONE;
		scene->world_state.level_state[revert_pos.level_index]->piece[revert_pos.level_position_1d] = player_val;
		scene->current_level = revert_pos.level_index;
	}
}

WorldPlayScene* world_player_action(WorldScene* scene, Direction action, Memory* level_memory)
{
	scene->last_action_was_teleport = false;
	//grab some useful information that we will reuse.
	IntPair move = direction_to_intpair(action);
	GameState* current_state = scene->world_state.level_state[scene->current_level];
	IntPair current_state_position = scene->world_state.level_position[scene->current_level];

	//Find the player.
	WorldPosition player_pos = world_maybe_find_player(scene);
	int current_player_value = scene->world_state.level_state[player_pos.level_index]->piece[player_pos.level_position_1d];
	/*
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
	*/	
	//calculate what square we are moving too.
	IntPair next_player_square_position;
	int next_square_level;
	{
		IntPair tentative_next = math_intpair_add(move, player_pos.level_position );
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
			for (int i = 0; i < scene->world_state.num_level; i++)
			{
				IntPair grid_location = math_intpair_sub(world_position, scene->world_state.level_position[i]);
				if (math_within_grid(grid_location.x, grid_location.y, scene->world_state.level_state[i]->w, scene->world_state.level_state[i]->h))
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
	GameState* next_state = scene->world_state.level_state[next_square_level];
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
			int current_state_position_1d = f2D(player_pos.level_position.x, player_pos.level_position.y, current_state->w, current_state->h);
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
					int link_w = scene->world_state.level_state[link_location]->w;
					int link_h = scene->world_state.level_state[link_location]->h;
					link_square_1d = f2D(link_square_2d.x, link_square_2d.y, link_w,link_h);
				}
				//remove the player from their current position, teleport them to our new position, and update the new final position for the player.
				{

					scene->last_action_was_teleport = false;
					next_state->piece[next_square_position_1d] = 0;
					scene->world_state.level_state[link_location]->piece[link_square_1d] = Piece::P_PLAYER;
					GameState* next = scene->world_state.level_state[next_square_level];
					int level_pos_1d = f2D(next_player_square_position.x, next_player_square_position.y, next->w, next->h);
					scene->staircase_we_entered_level_from = world_make_world_position(scene->current_level, next_player_square_position, level_pos_1d);
					scene->current_level = link_location;
					final_level_index = link_location;
					final_position_1d = link_square_1d;
				}
			}

		}
		
		//if the player is standing on a "pos level" tile OR the player just entered into a level staircase, initiate a new time_machine.
		{
			//Floor tile_player_standing_on = next_state->floor[next_square_position_1d];
			int tile_player_standing_on = scene->world_state.level_state[final_level_index]->floor[final_position_1d];
			bool standing_on_start_tile = tile_player_standing_on == F_START || tile_player_standing_on == F_STAIRCASE_LEVELSTART;
			if (standing_on_start_tile)
			{
				memory_clear(level_memory);
				GameState* next_scene_before_extrude = scene->world_state.level_state[scene->current_level];
				GameState* next_scene = gamestate_clone(next_scene_before_extrude, level_memory);
				gamestate_startup(next_scene);
				WorldPlayScene* result = (WorldPlayScene*) memory_alloc(level_memory, sizeof(WorldPlayScene));
				result->time_machine = gamestate_timemachine_create(next_scene, level_memory, 1024);
				result->draw_position = scene->world_state.level_position[final_level_index];
				return result;
			}
			else
			{
				scene->staircase_we_entered_level_from = world_make_world_position(-1, math_intpair_create(-1, -1), -1);
			}
		}
	}

	return NULL;


}
WorldScene* setup_world_scene_continue(WorldScene* scene, SCENE_TYPE go_to_on_backspace)
{
	scene->go_to_on_backspace = go_to_on_backspace;
	scene->last_action_was_teleport = false;
	return scene;
}
WorldScene* setup_world_scene(TimeMachineEditor* build_from, Memory* world_scene_memory, SCENE_TYPE go_to_on_backspace)
{
	WorldScene* result = (WorldScene*)memory_alloc(world_scene_memory, sizeof(WorldScene));
	result->go_to_on_backspace = go_to_on_backspace;
	result->last_action_was_teleport = false;
	const int num_gamestates = build_from->world_state.num_level;
	result->world_state.num_level = num_gamestates;
	for (int i = 0; i < num_gamestates; i++)
	{
		result->world_state.level_position[i] = build_from->world_state.level_position[i];
	}
	for (int i = 0; i < num_gamestates; i++)
	{
		result->world_state.level_solved[i] = build_from->world_state.level_solved[i];
	}
	for (int i = 0; i < num_gamestates; i++)
	{
		result->world_state.level_state[i] = gamestate_clone(build_from->world_state.level_state[i], world_scene_memory);
	}
	for (int i = 0; i < num_gamestates;i++)
	{
		result->world_state.level_names[i] = build_from->world_state.level_names[i];
	}
	for (int i = 0; i < num_gamestates;i++)
	{
		result->world_state.level_modes[i] = build_from->world_state.level_modes[i];
	}
	//find a player in our starter level.
	{
		bool done_finding_player = false;
		for (int i = 0; i < num_gamestates; i++)
		{
			int len = result->world_state.level_state[i]->w * result->world_state.level_state[i]->h;
			for (int z = 0; z < len; z++)
				if (result->world_state.level_state[i]->piece[z] == P_PLAYER)
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
	for (int i = 0; i < to_check->world_state.num_level; i++)
	{
		GameState* state = to_check->world_state.level_state[i];
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


WorldPosition world_maybe_find_player(WorldScene* scene)
{
	for (int z = 0; z < scene->world_state.num_level; z++)
	{
		int pos_1d = gamestate_maybe_find_player(scene->world_state.level_state[z]);
		if (pos_1d != -1)
		{
			IntPair pos_2d = t2D(pos_1d, scene->world_state.level_state[z]->w, scene->world_state.level_state[z]->h);
			WorldPosition result = world_make_world_position(z,pos_2d,pos_1d);
			return world_make_world_position(z,pos_2d,pos_1d);
		}
	}
	return world_make_world_position(-1,math_intpair_create(-1,-1),-1);
}

std::string world_serialize_old(WorldScene* world, Memory* scope, Memory* temp_memory)
{
	//serialize num levels.
	const int max_length = 10000;
	int output_consumed = 0;
	char* output = (char*)memory_alloc(temp_memory, sizeof(char) * max_length);
	const int num_gamestates = world->world_state.num_level;

	//parse num gamestates.
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "num_gamestates:%d;\n", num_gamestates);
	}
	//parse current level.
	//{
//		output_consumed += sprintf_s(output + output_consumed, max_length, "current_level:%d;\n", world->current_level);
//	}

	//positions:
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "positions:");
		for (int i = 0; i < num_gamestates; i++)
			output_consumed += sprintf_s(output + output_consumed, max_length, "%d,%d,", world->world_state.level_position[i].x, world->world_state.level_position[i].y);
		output_consumed += sprintf_s(output + output_consumed, max_length, ";\n");
	}
	
	//serialize all gamestate elements.
	parse_serialize_gamestate_layers(output, &output_consumed, max_length, num_gamestates, world->world_state.level_state);
	//serialize the gamestates names.
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "names:");
		for (int i = 0; i < num_gamestates; i++)
		{
			char* name = world->world_state.level_names[i].name;
			output_consumed += sprintf_s(output + output_consumed, max_length, "%s,", name);
		}
		output_consumed += sprintf_s(output + output_consumed, max_length, ";\n");
	}

	//ttttt
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "solved:");
		for (int i = 0; i < num_gamestates; i++)
		{
			output_consumed += sprintf_s(output + output_consumed, max_length, "%d,", world->world_state.level_solved[i]);
		}
		output_consumed += sprintf_s(output + output_consumed, max_length, ";\n");
	}
	return std::string(output);
}

WorldScene* world_deserialize(std::string world_string, Memory* scope, Memory* temp_memory)
{
	WorldState* world_state = parse_deserialize_timemachine(world_string, scope, temp_memory);
	WorldScene* result = (WorldScene*)memory_alloc(scope, sizeof(WorldScene));
	//put default values into the worldscene
	{
		for (int i = 0; i < MAX_NUMBER_GAMESTATES; i++)
		{
			result->world_state.level_state[i] = NULL;
		}
		for (int i = 0; i < MAX_NUMBER_GAMESTATES; i++)
			for(int j = 0; j < GAME_LEVEL_NAME_MAX_SIZE;j++)
			{
				result->world_state.level_names[i].name[j] = '\0';
			}
	}

	result->world_state = *world_state;
	result->current_level = world_maybe_find_player(result).level_index;
	result->staircase_we_entered_level_from = world_make_world_position_invalid();
	result->go_to_on_backspace = SCENE_TYPE::ST_COUNT; //default value, this is determined elsewhere.
	return result;
}

