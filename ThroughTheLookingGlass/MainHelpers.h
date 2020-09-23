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
#include "TextureAtlas.h"

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
struct GamefullspriteDrawInfo {
	int num_sprites_drawn;
	glm::vec4* atlas_mapper;
	glm::vec4* atlas_cpu;
	glm::mat4* final_cpu;
};

struct TextDrawInfo {
	glm::mat4* string_matrix_cpu;
	glm::vec4* string_atlas_cpu;
	TTF_Character* true_font_reference;
	AABB* text_positions;
	AABB* text_positions_normalized;
	int* current_number_drawn;
};
// settings
const int SCREEN_STARTING_WIDTH = 800;
const int SCREEN_STARTING_HEIGHT = 600;
const float SCREEN_RATIO = ((float)SCREEN_STARTING_WIDTH / (float)SCREEN_STARTING_HEIGHT);
const int MAX_NUM_FLOOR_SPRITES = 5000;
const int MAX_NUM_FULL_SPRITES = 100;

const float CAMERA_LERP_TIME = 0.8f;
const float CAMERA_ZOOM_SPEED = 3.0f;
const float CAMERA_MOVE_MULT = 0.02f;
//const float MAX_ZOOM = 3.0f;
const float MAX_ZOOM = 2.0f;
const float MAX_ZOOM_OUT_TO_STILL_DISPLAY_OUTLINES = 22.0f;
const int MAX_NUM_DOTTED_SPRITES = 200;
const int MAX_NUM_CHARACTERS = 200;
const float OUTLINE_DRAW_SIZE = 0.3f;

const int FONT_CHARACTER_HEIGHT = 128;	//used to be 48.
const int FONT_ATLAS_WIDTH = 1024;
const int FONT_ATLAS_HEIGHT = 1024;
const int FONT_NUM_CHARACTERS = 128;

const int max_actions_in_puzzle = 1024;

const float WAIT_BETWEEN_PLAYER_MOVE_REPEAT = 0.13f;
const float DRAW_TITLE_TIME = 2.0f;
//helper functions.

/************************************************************/
/**********draw functions************************************/
void draw_curse_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info);
void draw_stationary_piece_curses_to_gamespace(MovementAnimation* animation, DrawCurseAnimation* curse, GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index, float time_since_last_action);
void draw_piece_curses_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index, DrawCurseAnimation* curse, float time_since_last_action);
bool draw_animation_to_gamespace(MovementAnimation* animation, LayerDrawGPUData* info_array, int layer_index, float time_since_last_action);
void draw_layer_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index);
void draw_layers_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info);
void draw_gamestates_outlines_to_gamespace(
	GameState** gamestates,
	IntPair* offsets,
	int length_function_input,
	GamefullspriteDrawInfo* info,
	int index);
void draw_palette(IntPair palete_screen_start,
	GameSpaceCamera camera_game,
	ViewPortCamera camera_viewport,
	EditorUIState* ui_state,
	int palete_length,
	GamestateBrush* palete,
	LayerDrawGPUData* layer_draw);
void draw_black_box_over_screen(GameSpaceCamera screen, GamefullspriteDrawInfo* info);
void draw_outline_to_gamespace(AABB outline, GamefullspriteDrawInfo* info);

/***********************************************************/
/*********camera functions**********************************/
GameSpaceCamera math_camera_build_for_gamestate(GameState* gamestate, IntPair position, ViewPortCamera viewport, float x_padding = 4, float y_padding = 3);

/***********************************************************/
/***********************stateful utils**********************/
//highly stateful main action.
void take_player_action(Animations** maybe_animation, GamestateTimeMachine* maybe_time_machine, IntPair draw_position, EditorUIState* ui_state, Direction action, Memory* level_memory, Memory* frame_memory, Memory* animation_memory);
float get_text_to_screen_size(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextDrawInfo* info)
{
	//TODO: get info.current_number_drawn to replace the other number we were using.
	glm::vec3 next_position = start_position;
	int draw = *info->current_number_drawn;
	for (; c_string[0]; c_string++)
	{
		char c = c_string[0];

		//handle the position of the character.
		float horizontal_space = info->text_positions[c].w / FONT_CHARACTER_HEIGHT;

		float x_start = (next_position.x + info->true_font_reference[c].Bearing.x) / float(FONT_CHARACTER_HEIGHT);
		float y_start = (next_position.y - (info->true_font_reference[c].Size.y - info->true_font_reference[c].Bearing.y)) / float(FONT_CHARACTER_HEIGHT);
		float w = (info->true_font_reference[c].Size.x) / float(FONT_CHARACTER_HEIGHT);
		float h = info->true_font_reference[c].Size.y / float(FONT_CHARACTER_HEIGHT);

		//we always increment the draw foward, even when we don't draw stuff like an empty space.
		float x_pixels_in_gamespace = (info->true_font_reference[c].advance >> 6) / float(FONT_CHARACTER_HEIGHT);
		next_position.x += (x_pixels_in_gamespace * scale.x);

	}
	*info->current_number_drawn = draw;
	return next_position.x - start_position.x;
}
float draw_text_to_screen(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextDrawInfo* info);
void draw_text_maximized_centered_to_screen(GameSpaceCamera camera, char* string_to_draw, TextDrawInfo* text_draw_info)
{
	//draw the text.
	glm::vec3 screen_center = glm::vec3((camera.left + camera.right) / 2.0f, (camera.up + camera.down) / 2.0f, 15);

	float text_width = get_text_to_screen_size(screen_center, glm::vec2(1, 1), string_to_draw, text_draw_info);
	float x_dist = camera.right - camera.left;
	float x_scale = x_dist / text_width;
	float x_center = (camera.right + camera.left) / 2.0f;
	float x_start = screen_center.x - (text_width * x_scale) / 2.0f;
	draw_text_to_screen(glm::vec3(x_start, screen_center.y, screen_center.z), glm::vec2(x_scale, x_scale), string_to_draw, text_draw_info);
}
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
IntPair calculate_floor_cell_clicked(GameState* currentState, IntPair position, glm::vec2 mouseGamePos);

/********************************************************************/
/**************************text utils********************************/

float get_text_to_screen_size(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextDrawInfo* info);