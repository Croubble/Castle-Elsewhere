#include "Click.h"
#include "Constants.h"

void button_click_state_set_all_false(button_click_state* state)
{
	state->pressed = false;
	state->pressed_this_frame = false;
	state->released_this_frame = false;
	state->released_since_pressed_last = true;
	state->time_pressed = 0;
}

GAME_ACTION_NAME internal_string_to_name(const char* name)
{
	GAME_ACTION_NAME result;
	int i;
	for (i = 0; name[i] != '\0' && i < 32; i++)
	{
		result.name[i] = name[i];
	}
	result.name[i] = '\0';
	if (i > 32)
	{
		crash_err("one of our GAME_ACTIONS is greater than the total length of a game-action-name, just increase that const char value. Its fine.");
	}
	return result;
}
GAME_ACTION_NAME click_gameaction_to_name(GAME_ACTION action)
{
	int length = 11;
	if (length != G_LENGTH)
	{
		crash_err("you need to update the GAME_ACTION to string binding. Yeah. I know. Sucks that you just can't auto convert. The price of C. \0");
	}
	if (action == G_UP)
		return internal_string_to_name("G_UP");
	if (action == G_DOWN)
		return internal_string_to_name("DOWN");
	if (action == G_LEFT)
		return internal_string_to_name("LEFT");
	if (action == G_RIGHT)
		return internal_string_to_name("RIGHT");
	if(action == G_UNDO)
		return internal_string_to_name("UNDO");
	if(action == G_RESET)
		return internal_string_to_name("RESET");
	if(action == G_EXIT_LEVEL)
		return internal_string_to_name("EXIT LEVEL");
	if(action == G_BACK_MENU)
		return internal_string_to_name("BACK MENU");
	if(action == G_MENU_UP)
		return internal_string_to_name("MENU UP");
	if(action == G_MENU_DOWN)
		return internal_string_to_name("MENU DOWN");
	if(action == G_MENU_ENTER)
		return internal_string_to_name("MENU ENTER");
	crash_err("???? you might have passed in a wrong value to the function, or the function doesn't properly catch one of the cases.");
	return internal_string_to_name("BUGS!");
}
button_click_state click_test_button(EditorUIState* ui_state, GAME_ACTION action)
{
	//int keycode_prime = ui_state->button_mapping.primary_mapping;
	//int keycode_secondary = 
	//int result = click_sdl_keycode_to_index_position(int sdlk_keycode);
	//MappingState state;
	crash_err("function unfinished, you shouldn't be calling this bad boy");
	button_click_state result;
	result.pressed = false;
	result.pressed_this_frame = false;
	result.released_since_pressed_last = false;
	result.time_pressed = 0;
	return result;
}
MappingState click_make_default_mapping_state()
{
	MappingState result;
	for (int i = 0; i < G_LENGTH; i++)
	{
		result.primary_mapping[i] = SDLK_UNKNOWN;
		result.secondary_mapping[i] = SDLK_UNKNOWN;
	}
	result.primary_mapping[G_UP] = SDLK_UP;
	result.primary_mapping[G_DOWN] = SDLK_DOWN;
	result.primary_mapping[G_LEFT] = SDLK_LEFT;
	result.primary_mapping[G_RIGHT] = SDLK_RIGHT;
	result.primary_mapping[G_UNDO] = SDLK_z;
	result.primary_mapping[G_RESET] = SDLK_r;
	result.primary_mapping[G_EXIT_LEVEL] = SDLK_q;
	result.primary_mapping[G_BACK_MENU] = SDLK_BACKSPACE;
	result.primary_mapping[G_MENU_UP] = SDLK_w;
	result.primary_mapping[G_MENU_DOWN] = SDLK_s;
	result.primary_mapping[G_MENU_ENTER] = SDLK_KP_ENTER;

	result.secondary_mapping[G_UP] = SDLK_w;
	result.secondary_mapping[G_DOWN] = SDLK_s;
	result.secondary_mapping[G_LEFT] = SDLK_a;
	result.secondary_mapping[G_RIGHT] = SDLK_d;
	result.secondary_mapping[G_UNDO] = SDLK_z;
	result.secondary_mapping[G_RESET] = SDLK_UNKNOWN;
	result.secondary_mapping[G_EXIT_LEVEL] = SDLK_UNKNOWN;
	result.secondary_mapping[G_BACK_MENU] = SDLK_UNKNOWN;
	result.secondary_mapping[G_MENU_UP] = SDLK_UP;
	result.secondary_mapping[G_MENU_DOWN] = SDLK_DOWN;
	result.secondary_mapping[G_MENU_ENTER] = SDLK_UNKNOWN;

	return result;
}

EditorUIState click_ui_init(Memory* permanent_memory)
{
	EditorUIState result;

	result.time_since_scene_started = 0;
	result.time_till_player_can_move = 0;
	result.time_since_last_player_action = 0;

	result.most_recently_pressed_direction = ' ';
	result.button_mapping = click_make_default_mapping_state();
	for (int i = 0; i < NUM_SDLK_KEYCODES; i++)
	{
		result.key_values[i].pressed = false;
		result.key_values[i].pressed_this_frame = false;
		result.key_values[i].released_this_frame = false;
		result.key_values[i].released_since_pressed_last = true;
		result.key_values[i].time_pressed = 0;
	}
	button_click_state_set_all_false(&result.left);
	button_click_state_set_all_false(&result.right);
	button_click_state_set_all_false(&result.down);
	button_click_state_set_all_false(&result.up);
	button_click_state_set_all_false(&result.spacebar);
	button_click_state_set_all_false(&result.enter);
	int x = SDLK_UNDERSCORE;
	int z = SDLK_RIGHTPAREN;
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


int click_sdl_keycode_to_index_position(int sdlk_keycode)
{
	for (int i = 0; i < NUM_SDLK_KEYCODES; i++)
	{
		if (all_sdl_keycodes[i] == sdlk_keycode)
		{
			return i;
		}
	}
	crash_err("we tried to lookup a sdlk keycode, but it didn't match any sdlkeycodes we know about. This should never happen. crash!");
	return 0;
};


button_click_state* get_keycode_state(EditorUIState* state, SDL_KeyCode keycode)
{
	return &state->key_values[click_sdl_keycode_to_index_position(keycode)];
}
