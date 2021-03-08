#pragma once
#include "Math.h"
#include "GameState.h"
#include "Editor.h"
#include "Animation.h"
#include "Constants.h"
#include <iostream>

struct WorldScene
{
	SCENE_TYPE go_to_on_backspace;
	int num_levels;
	int current_level;
	char level_names[MAX_NUMBER_GAMESTATES * GAME_LEVEL_NAME_MAX_SIZE];
	GameState* level_state[MAX_NUMBER_GAMESTATES];
	IntPair level_position[MAX_NUMBER_GAMESTATES];
	bool level_solved[MAX_NUMBER_GAMESTATES];
};

struct WorldPlayScene
{
	IntPair draw_position;
	GamestateTimeMachine* time_machine;
};

WorldPlayScene* world_player_action(WorldScene* scene, Direction action, Memory* level_memory);
WorldScene* setup_world_scene(TimeMachineEditor* build_from, Memory* world_scene_memory, SCENE_TYPE go_to_on_backspace);
bool any_levels_left_active(WorldScene* to_check);
std::string world_serialize(WorldScene* world, Memory* scope, Memory* temp);
WorldScene* world_deserialize(std::string world, Memory* scope, Memory* temp);
