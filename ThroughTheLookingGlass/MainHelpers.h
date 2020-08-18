#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GameState.h"
#include "Math.h"
#include "Shader.h"
#include "Editor.h"
#include <iostream>
#include "Resource.h"
#include <SDL.h>
#include "Clock.h"
#include "Click.h"
#include "GenericGamestate.h"
#include "TrueFontCharacter.h"
#include <ft2build.h>
#include "Parse.h"
#include "World.h"
#include "Animation.h"
#include FT_FREETYPE_H  

enum SCENE_TYPE
{
	ST_EDITOR,
	ST_EDIT_LEVEL,
	ST_PLAY_WORLD,
	ST_PLAY_LEVEL,
	ST_COUNT
};

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
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float SCREEN_RATIO = ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
const int MAX_NUM_FLOOR_SPRITES = 5000;
const int MAX_NUM_FULL_SPRITES = 100;

const float CAMERA_LERP_TIME = 0.8f;
const float CAMERA_ZOOM_SPEED = 3.0f;
const float CAMERA_MOVE_MULT = 0.02f;
//const float MAX_ZOOM = 3.0f;
const float MAX_ZOOM = 0.5f;
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
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void draw_animation_to_gamespace(Animation* animation, LayerDrawGPUData* info_array, int layer_index, float time_since_last_action);
void draw_layer_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index);
void draw_layers_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info);

void draw_gamestates_outlines_to_gamespace(
	GameState** gamestates,
	IntPair* offsets,
	int length_function_input,
	GamefullspriteDrawInfo* info,
	int index);

GameSpaceCamera math_camera_build_for_gamestate(GameState* gamestate, IntPair position, ViewPortCamera viewport, float x_padding = 4, float y_padding = 3);
/*
float draw_text_to_screen(glm::vec3 start_position,
	const char* c_string,
	glm::mat4* matrix,
	glm::vec4* atlas_output,
	TTF_Character* true_font_reference,
	AABB* atlas_reference,
	AABB* normalized_atlas_reference,
	int* current_draw,
	float num_pixels_per_gameunit);
	*/
void maybe_take_player_action(WorldScene* world_scene_state, EditorUIState* ui_state, char letter_to_test, Direction action, Memory* level_memory, Memory* frame_memory, Memory* animation_memory);
float get_text_to_screen_size(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextDrawInfo* info);
float draw_text_to_screen(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextDrawInfo* info);
WorldScene* setup_world_scene(TimeMachineEditor* build_from, Memory* world_scene_memory);
AABB calculate_outline_from_create_info(Memory* frame_memory, TimeMachineEditor* timeMachine, EditorUIState ui_state);
bool MaybeApplyBrush(GamestateBrush* palete, int currentBrush, EditorUIState* ui_state, TimeMachineEditor* timeMachine, glm::vec2 mouseGamePos);
bool MaybeApplyBrushInPlayMode(Memory* memory, GamestateBrush* palete, int current_brush, EditorUIState* ui_state, GamestateTimeMachine* time_machine, IntPair current_state_pos, glm::vec2 mouseGamePos);
void draw_black_box_over_screen(GameSpaceCamera screen, GamefullspriteDrawInfo* info);
void draw_outline_to_gamespace(AABB outline, GamefullspriteDrawInfo* info);
AABB calculate_outline_position_from_drag_info(Memory* frame_memory,
	TimeMachineEditor* timeMachine,
	EditorUIState ui_state,
	glm::vec2 dragging_start_position_in_gamespace);
AABB calculate_outline_from_move_info(Memory* frame_memory, TimeMachineEditor* timeMachine, EditorUIState ui_state);
IntPair calculate_floor_cell_clicked(GameState* currentState, IntPair position, glm::vec2 mouseGamePos);
TextureAtlasMaps* textureatlas_catoban_create(Memory* memory);
TextureAtlasMaps* textureatlas_create(Memory* memory, char*** texture_references);


void draw_palette(IntPair palete_screen_start,
	GameSpaceCamera camera_game,
	ViewPortCamera camera_viewport,
	EditorUIState* ui_state,
	int palete_length,
	GamestateBrush* palete,
	LayerDrawGPUData* layer_draw);