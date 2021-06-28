#pragma once
#include "Math.h"
#include "GameState.h"
#include "Editor.h"
#include "Animation.h"
#include "Constants.h"
#include <iostream>

struct WorldPosition
{
	int level_index;
	IntPair level_position;
	int level_position_1d;
};
struct WorldScene
{
	//data that is used for moving that player around.
	SCENE_TYPE go_to_on_backspace;
	WorldPosition staircase_we_entered_level_from;
	int current_level;

	//data that is just the state our 
	WorldState world_state;
};

struct WorldPlayScene
{
	IntPair draw_position;
	GamestateTimeMachine* time_machine;
};

void world_try_reversing_staircase(WorldScene* scene);
WorldPosition world_make_world_position_invalid();
WorldPosition world_make_world_position(int level_index, IntPair pos_2d, int pos_1d);
WorldPlayScene* world_player_action(WorldScene* scene, Direction action, Memory* level_memory);
WorldScene* setup_world_scene(TimeMachineEditor* build_from, Memory* world_scene_memory, SCENE_TYPE go_to_on_backspace);
bool any_levels_left_active(WorldScene* to_check);
std::string world_serialize(WorldScene* world, Memory* scope, Memory* temp);
WorldScene* world_deserialize(std::string world, Memory* scope, Memory* temp);
WorldPosition world_maybe_find_player(WorldScene* scene);
