#pragma once
#include "Math.h"
#include <SDL.h>
const int GAME_HEIGHT_START = 12;
const int NUM_LETTERS_ON_KEYBOARD = SDLK_z - SDLK_a + 1;
const int MAX_NUM_KEYCODES = 256;
enum EditorClickStateType
{
	ECS_NEUTRAL,
	ECS_BRUSH,
	ECS_RESIZE_GAMESTATE,
	ECS_MOVE_GAMESTATE,
	ECS_CREATE_GAMESTATE,
	ECS_EDIT_NAME,
	ECS_COUNT
};

enum GAME_ACTION
{
	G_UP,
	G_DOWN,
	G_LEFT,
	G_RIGHT,
	G_UNDO,
	G_RESET,
	G_EXIT_LEVEL,
	G_BACK_MENU,
	G_MENU_UP,
	G_MENU_DOWN,
	G_MENU_ENTER,
	G_LENGTH,
	G_ERROR
};
struct NeutralClickState
{
	int x;
};
struct BrushClickState
{
	int y;
};
struct ResizeClickState
{
	bool dragging_left;
	bool dragging_right;
	bool dragging_up;
	bool dragging_down;
	int dragging_gamestate_index;
};
struct MoveClickState
{
	int moving_gamestate_index;
	glm::vec2 move_start_position;
};
struct CreateGamestateClickState
{
	IntPair gameworld_start_pos;
};

struct button_click_state
{
	//SDL_KeyCode
	bool pressed_this_frame;
	bool released_this_frame;
	bool pressed;
	bool released_since_pressed_last;
	float time_pressed;
};

struct MappingState
{
	SDL_KeyCode primary_mapping[G_LENGTH];
	SDL_KeyCode secondary_mapping[G_LENGTH];
	//SDLK_LENGTH;
};
struct EditorUIState
{
	bool update_actual_screen_size;
	glm::ivec2 next_camera_size;
	float time_since_scene_started;
	float time_till_player_can_move;
	float time_since_last_player_action;
	float total_time_passed;
	float game_height_current;

	char most_recently_pressed_direction;
	button_click_state* letters;

	button_click_state left;
	button_click_state right;
	button_click_state up;
	button_click_state down;

	button_click_state spacebar;
	button_click_state enter;
	EditorClickStateType type;

	bool click_left_down_this_frame;
	bool click_left_up_this_frame;
	bool mouse_left_click_down;
	bool mouse_right_click_down;
	bool shift_key_down_this_frame;
	bool shift_key_up_this_frame;
	bool shift_key_down;
	bool control_key_down;
	bool alt_key_down;
	bool z_key_down_this_frame;
	float wheel_move;
	bool backspace_key_down_this_frame;

	//mouse position info.
	glm::vec2 mouseGamePos;
	IntPair mouse_last_pos;
	IntPair mousePos;
	IntPair totalMove;
	union {
		NeutralClickState neutral;
		BrushClickState brush;
		ResizeClickState resize;
		MoveClickState move;
		CreateGamestateClickState create;
	} un;
};


MappingState click_make_default_mapping_state();
void button_click_state_set_all_false(button_click_state* state);
EditorUIState click_ui_init(Memory* permanent_memory);
