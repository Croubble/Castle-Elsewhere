#include "Click.h"

void button_click_state_set_all_false(button_click_state* state)
{
	state->pressed = false;
	state->pressed_this_frame = false;
	state->released_this_frame = false;
	state->released_since_pressed_last = true;
	state->time_pressed = 0;
}

EditorUIState click_ui_init(Memory* permanent_memory)
{
	EditorUIState result;

	result.time_since_scene_started = 0;
	result.time_till_player_can_move = 0;
	result.time_since_last_player_action = 0;

	result.most_recently_pressed_direction = ' ';
	result.letters = (button_click_state*)memory_alloc(permanent_memory, sizeof(button_click_state) * NUM_LETTERS_ON_KEYBOARD);
	for (int i = 0; i < NUM_LETTERS_ON_KEYBOARD; i++)
	{
		result.letters[i].pressed = false;
		result.letters[i].pressed_this_frame = false;
		result.letters[i].released_this_frame = false;
		result.letters[i].released_since_pressed_last = true;
		result.letters[i].time_pressed = 0;
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
	result.shift_key_down_this_frame = false;
	result.shift_key_up_this_frame = false;
	result.shift_key_down = false;
	result.control_key_down = false;
	result.alt_key_down = false;
	result.z_key_down_this_frame = false;
	result.backspace_key_down_this_frame = false;

	return result;
}
