#pragma once
#include "Math.h"
#include "GameState.h"
#include "Editor.h"
#include "Animation.h"
#include "Constants.h"
#include <iostream>

struct WorldScene
{
	//data that is used for moving that player around.
	SCENE_TYPE go_to_on_backspace;
	WorldPosition staircase_we_entered_level_from;
	int current_level;

	bool last_action_was_teleport;
	//data that is just the state our 
	WorldState world_state;
};

struct WorldPlayScene
{
	IntPair draw_position;
	GamestateTimeMachine* time_machine;
};

void world_try_reversing_staircase(WorldScene* scene);
WorldPlayScene* world_player_action(WorldScene* scene, Direction action, Memory* level_memory);
WorldScene* setup_world_scene(TimeMachineEditor* build_from, Memory* world_scene_memory, SCENE_TYPE go_to_on_backspace);
bool any_levels_left_active(WorldScene* to_check);
WorldScene* world_deserialize(std::string world, Memory* scope, Memory* temp);
WorldPosition world_maybe_find_player(WorldScene* scene);
