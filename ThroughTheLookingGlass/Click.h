#pragma once
#include "Math.h"
#include <SDL.h>
const int GAME_HEIGHT_START = 12;
const int NUM_LETTERS_ON_KEYBOARD = SDLK_z - SDLK_a + 1;
const int NUM_SDLK_KEYCODES = 255;
const int NUM_KEYS_TO_TEST = 5;

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
	button_click_state key_values[NUM_SDLK_KEYCODES];
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

SDL_KeyCode* click_get_all_keycodes();
MappingState click_make_default_mapping_state();
void button_click_state_set_all_false(button_click_state* state);
EditorUIState click_ui_init(Memory* permanent_memory);
int click_sdl_keycode_to_index_position(int sdlk_keycode);
const int all_sdl_keycodes[] = {
	SDLK_0,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_4,
	SDLK_5,
	SDLK_6,
	SDLK_7,
	SDLK_8,
	SDLK_9,
	SDLK_a,
	SDLK_AC_BACK,
	SDLK_AC_BOOKMARKS,
	SDLK_AC_FORWARD,
	SDLK_AC_HOME,
	SDLK_AC_REFRESH,
	SDLK_AC_SEARCH,
	SDLK_AC_STOP,
	SDLK_AGAIN,
	SDLK_ALTERASE,
	SDLK_QUOTE,
	SDLK_APPLICATION,
	SDLK_AUDIOMUTE,
	SDLK_AUDIONEXT,
	SDLK_AUDIOPLAY,
	SDLK_AUDIOPREV,
	SDLK_AUDIOSTOP,
	SDLK_b,
	SDLK_BACKSLASH,
	SDLK_BACKSPACE,
	SDLK_BRIGHTNESSDOWN,
	SDLK_BRIGHTNESSUP,
	SDLK_c,
	SDLK_CALCULATOR,
	SDLK_CANCEL,
	SDLK_CAPSLOCK,
	SDLK_CLEAR,
	SDLK_CLEARAGAIN,
	SDLK_COMMA,
	SDLK_COMPUTER,
	SDLK_COPY,
	SDLK_CRSEL,
	SDLK_CURRENCYSUBUNIT,
	SDLK_CURRENCYUNIT,
	SDLK_CUT,
	SDLK_d,
	SDLK_DECIMALSEPARATOR,
	SDLK_DELETE,
	SDLK_DISPLAYSWITCH,
	SDLK_DOWN,
	SDLK_e,
	SDLK_EJECT,
	SDLK_END,
	SDLK_EQUALS,
	SDLK_ESCAPE,
	SDLK_EXECUTE,
	SDLK_EXSEL,
	SDLK_f,
	SDLK_F1,
	SDLK_F10,
	SDLK_F11,
	SDLK_F12,
	SDLK_F13,
	SDLK_F14,
	SDLK_F15,
	SDLK_F16,
	SDLK_F17,
	SDLK_F18,
	SDLK_F19,
	SDLK_F2,
	SDLK_F20,
	SDLK_F21,
	SDLK_F22,
	SDLK_F23,
	SDLK_F24,
	SDLK_F3,
	SDLK_F4,
	SDLK_F5,
	SDLK_F6,
	SDLK_F7,
	SDLK_F8,
	SDLK_F9,
	SDLK_FIND,
	SDLK_g,
	SDLK_BACKQUOTE,
	SDLK_h,
	SDLK_HELP,
	SDLK_HOME,
	SDLK_i,
	SDLK_INSERT,
	SDLK_j,
	SDLK_k,
	SDLK_KBDILLUMDOWN,
	SDLK_KBDILLUMTOGGLE,
	SDLK_KBDILLUMUP,
	SDLK_KP_0,
	SDLK_KP_00,
	SDLK_KP_000,
	SDLK_KP_1,
	SDLK_KP_2,
	SDLK_KP_3,
	SDLK_KP_4,
	SDLK_KP_5,
	SDLK_KP_6,
	SDLK_KP_7,
	SDLK_KP_8,
	SDLK_KP_9,
	SDLK_KP_A,
	SDLK_KP_AMPERSAND,
	SDLK_KP_AT,
	SDLK_KP_B,
	SDLK_KP_BACKSPACE,
	SDLK_KP_BINARY,
	SDLK_KP_C,
	SDLK_KP_CLEAR,
	SDLK_KP_CLEARENTRY,
	SDLK_KP_COLON,
	SDLK_KP_COMMA,
	SDLK_KP_D,
	SDLK_KP_DBLAMPERSAND,
	SDLK_KP_DECIMAL,
	SDLK_KP_DIVIDE,
	SDLK_KP_E,
	SDLK_KP_ENTER,
	SDLK_KP_EQUALS,
	SDLK_KP_EQUALSAS400,
	SDLK_KP_EXCLAM,
	SDLK_KP_F,
	SDLK_KP_GREATER,
	SDLK_KP_HASH,
	SDLK_KP_HEXADECIMAL,
	SDLK_KP_LEFTBRACE,
	SDLK_KP_LEFTPAREN,
	SDLK_KP_LESS,
	SDLK_KP_MEMADD,
	SDLK_KP_MEMCLEAR,
	SDLK_KP_MEMDIVIDE,
	SDLK_KP_MEMMULTIPLY,
	SDLK_KP_MEMRECALL,
	SDLK_KP_MEMSTORE,
	SDLK_KP_MEMSUBTRACT,
	SDLK_KP_MINUS,
	SDLK_KP_MULTIPLY,
	SDLK_KP_OCTAL,
	SDLK_KP_PERCENT,
	SDLK_KP_PERIOD,
	SDLK_KP_PLUS,
	SDLK_KP_PLUSMINUS,
	SDLK_KP_POWER,
	SDLK_KP_RIGHTBRACE,
	SDLK_KP_RIGHTPAREN,
	SDLK_KP_SPACE,
	SDLK_KP_TAB,
	SDLK_KP_VERTICALBAR,
	SDLK_KP_XOR,
	SDLK_l,
	SDLK_LALT,
	SDLK_LCTRL,
	SDLK_LEFT,
	SDLK_LEFTBRACKET,
	SDLK_LGUI,
	SDLK_LSHIFT,
	SDLK_m,
	SDLK_MAIL,
	SDLK_MEDIASELECT,
	SDLK_MENU,
	SDLK_MINUS,
	SDLK_MODE,
	SDLK_MUTE,
	SDLK_n,
	SDLK_NUMLOCKCLEAR,
	SDLK_o,
	SDLK_OPER,
	SDLK_OUT,
	SDLK_p,
	SDLK_PAGEDOWN,
	SDLK_PAGEUP,
	SDLK_PASTE,
	SDLK_PAUSE,
	SDLK_PERIOD,
	SDLK_POWER,
	SDLK_PRINTSCREEN,
	SDLK_PRIOR,
	SDLK_q,
	SDLK_r,
	SDLK_RALT,
	SDLK_RCTRL,
	SDLK_RETURN,
	SDLK_RETURN2,
	SDLK_RGUI,
	SDLK_RIGHT,
	SDLK_RIGHTBRACKET,
	SDLK_RSHIFT,
	SDLK_s,
	SDLK_SCROLLLOCK,
	SDLK_SELECT,
	SDLK_SEMICOLON,
	SDLK_SEPARATOR,
	SDLK_SLASH,
	SDLK_SLEEP,
	SDLK_SPACE,
	SDLK_STOP,
	SDLK_SYSREQ,
	SDLK_t,
	SDLK_TAB,
	SDLK_THOUSANDSSEPARATOR,
	SDLK_u,
	SDLK_UNDO,
	SDLK_UNKNOWN,
	SDLK_UP,
	SDLK_v,
	SDLK_VOLUMEDOWN,
	SDLK_VOLUMEUP,
	SDLK_w,
	SDLK_WWW,
	SDLK_x,
	SDLK_y,
	SDLK_z,
	SDL_SCANCODE_INTERNATIONAL1,
	SDL_SCANCODE_INTERNATIONAL2,
	SDL_SCANCODE_INTERNATIONAL3,
	SDL_SCANCODE_INTERNATIONAL4,
	SDL_SCANCODE_INTERNATIONAL5,
	SDL_SCANCODE_INTERNATIONAL6,
	SDL_SCANCODE_INTERNATIONAL7,
	SDL_SCANCODE_INTERNATIONAL8,
	SDL_SCANCODE_INTERNATIONAL9,
	SDL_SCANCODE_LANG1,
	SDL_SCANCODE_LANG2,
	SDL_SCANCODE_LANG3,
	SDL_SCANCODE_LANG4,
	SDL_SCANCODE_LANG5,
	SDL_SCANCODE_LANG6,
	SDL_SCANCODE_LANG7,
	SDL_SCANCODE_LANG8,
	SDL_SCANCODE_LANG9,
	SDL_SCANCODE_NONUSBACKSLASH,
	SDL_SCANCODE_NONUSHASH,
	SDLK_AMPERSAND,
	SDLK_ASTERISK,
	SDLK_AT,
	SDLK_CARET,
	SDLK_COLON,
	SDLK_DOLLAR,
	SDLK_EXCLAIM,
	SDLK_GREATER,
	SDLK_HASH,
	SDLK_LEFTPAREN,
	SDLK_LESS,
	SDLK_PERCENT,
	SDLK_PLUS,
	SDLK_QUESTION,
	SDLK_QUOTEDBL,
	SDLK_RIGHTPAREN,
	SDLK_UNDERSCORE
};
