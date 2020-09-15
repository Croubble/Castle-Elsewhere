#pragma once
#include "Math.h"
#include "GameState.h"
#include "Editor.h"
#include "Animation.h"
#include "Constants.h"
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

Animations* animation_build_from_world(GameActionJournal* journal, GamestateTimeMachine* maybe_time_machine, IntPair draw_position, Memory* animation_memory);
void world_player_action(WorldScene* scene, Direction action, Memory* level_memory);
WorldScene* setup_world_scene(TimeMachineEditor* build_from, Memory* world_scene_memory);
