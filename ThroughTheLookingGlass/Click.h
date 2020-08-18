#pragma once

const int GAME_HEIGHT_START = 12.5f;
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
};

struct EditorUIState
{
	float time_since_scene_started;
	float time_till_player_can_move;
	float time_since_last_player_action;
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

void button_click_state_set_all_false(button_click_state* state)
{
	state->pressed = false;
	state->pressed_this_frame = false;
	state->released_this_frame = false;
	state->released_since_pressed_last = true;
}
EditorUIState click_ui_init(Memory* permanent_memory)
{
	EditorUIState result;

	result.time_since_scene_started = 0;
	result.time_till_player_can_move = 0;
	result.time_since_last_player_action = 0;

	result.most_recently_pressed_direction = ' ';
	result.letters = (button_click_state*) memory_alloc(permanent_memory, sizeof(button_click_state) * NUM_LETTERS_ON_KEYBOARD);
	for (int i = 0; i < NUM_LETTERS_ON_KEYBOARD;i++)
	{
		result.letters[i].pressed = false;
		result.letters[i].pressed_this_frame = false;
		result.letters[i].released_this_frame = false;
		result.letters[i].released_since_pressed_last = true;
	}
	button_click_state_set_all_false(&result.left);
	button_click_state_set_all_false(&result.right);
	button_click_state_set_all_false(&result.down);
	button_click_state_set_all_false(&result.up);
	button_click_state_set_all_false(&result.spacebar);
	button_click_state_set_all_false(&result.enter);
	result.game_height_current = GAME_HEIGHT_START;
	result.type = ECS_NEUTRAL;
	result.mouseGamePos = glm::vec2(0, 0);
	result.mouse_last_pos = math_intpair_create(0, 0);
	result.mousePos = math_intpair_create(0, 0);
	result.totalMove = math_intpair_create(0, 0);
	result.mouse_left_click_down = false;
	result.mouse_right_click_down = false;
	result.un.neutral.x = 0;
	result.shift_key_down_this_frame= false;
	result.shift_key_up_this_frame = false;
	result.shift_key_down = false;
	result.control_key_down = false;
	result.alt_key_down = false;
	result.z_key_down_this_frame = false;
	result.backspace_key_down_this_frame = false;

	return result;
}

