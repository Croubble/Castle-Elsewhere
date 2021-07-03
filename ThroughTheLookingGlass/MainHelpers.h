#pragma once
#include <cstring>
#include "GameState.h"
#include "Math.h"
#include "Shader.h"
#include "Editor.h"
#include <iostream>
#include "Resource.h"
#include <SDL.h>
#include "Click.h"
#include "TrueFontCharacter.h"
#include <ft2build.h>
#include "Parse.h"
#include "World.h"
#include "Animation.h"
#include "TextDraw.h"
#include "HelperGL.h"
#include "draw.h"
#include FT_FREETYPE_H  



struct PlayScene
{
	GamestateTimeMachine* timeMachine;
	GamestateTimeMachine* timeMachine_edit;
	IntPair loc;
	IntPair loc_edit;
	int game_name_length;
	char game_name[GAME_LEVEL_NAME_MAX_SIZE];
	int editor_position_in_time_machine;
};


//helper structs.
struct GamespriteDrawInfo {
	glm::vec4* floor_atlas_cpu;
	glm::vec3* floor_positions_cpu;
	int* floor_total_drawn;
	glm::vec4* floor_atlas_mapper;
	glm::vec4* piece_atlas_cpu;
	glm::vec3* piece_positions_cpu;
	int* piece_total_drawn;
	glm::vec4* piece_atlas_mapper;
};



//helper functions.

/************************************************************/
/**********draw functions************************************/

/***********************************************************/
/*********camera functions**********************************/
GameSpaceCamera math_camera_build_for_gamestate(GameState* gamestate, IntPair position, ViewPortCamera viewport, float x_padding = 4, float y_padding = 3);

/***********************************************************/
/***********************stateful utils**********************/
//highly stateful main action.
void take_player_action(GamestateTimeMachine* maybe_time_machine, IntPair draw_position, EditorUIState* ui_state, Direction action, Memory* level_memory, Memory* frame_memory, Memory* animation_memory);


bool MaybeApplyBrush(GamestateBrush* palete, int currentBrush, EditorUIState* ui_state, TimeMachineEditor* timeMachine, glm::vec2 mouseGamePos);
bool MaybeApplyBrushInPlayMode(Memory* memory, GamestateBrush* palete, int current_brush, EditorUIState* ui_state, GamestateTimeMachine* time_machine, IntPair current_state_pos, glm::vec2 mouseGamePos);

/********************************************************************/
/************************click calculations utils********************/
AABB calculate_outline_from_create_info(Memory* frame_memory, TimeMachineEditor* timeMachine, EditorUIState ui_state);
AABB calculate_outline_position_from_drag_info(Memory* frame_memory,
	TimeMachineEditor* timeMachine,
	EditorUIState ui_state,
	glm::vec2 dragging_start_position_in_gamespace);
AABB calculate_outline_from_move_info(Memory* frame_memory, TimeMachineEditor* timeMachine, EditorUIState ui_state);

/********************************************************************/
/**************************text utils********************************/

