#include "MainHelpers.h"
#include <iostream>
#include <sstream>
#include "string.h"
#include "SaveLoad.h"
#include "TextScene.h"
#include "MenuScene.h"
#include "EditorScene.h"

void HandleSharedEvents(EditorUIState* ui_state, GameSpaceCamera* camera_game, glm::mat4* camera, bool mouse_moved_this_frame, SCENE_TYPE scene)
{
	bool left_click_action_resolved = false;
	if (ui_state->wheel_move != 0 && (scene == SCENE_TYPE::ST_EDITOR || scene == SCENE_TYPE::ST_EDIT_LEVEL))
	{
		//scroll up!
		std::cout << "begin printing zoom info" << std::endl;
		float game_height_old = ui_state->game_height_current;
		ui_state->game_height_current -= ui_state->wheel_move;
		ui_state->game_height_current = maxf(MAX_ZOOM, ui_state->game_height_current);
		float ratio = ui_state->game_height_current / game_height_old;
		float camera_center_x = (camera_game->left + camera_game->right) / 2.0f;
		float camera_center_y = (camera_game->up + camera_game->down) / 2.0f;
		float camera_width = (camera_game->right - camera_game->left);
		float camera_height = (camera_game->up - camera_game->down);
		float new_camera_half_width = (camera_width / 2.0f) * ratio;
		float new_camera_half_height = (camera_height / 2.0f) * ratio;

		camera_game->left = camera_center_x - new_camera_half_width;
		camera_game->right = camera_center_x + new_camera_half_width;
		camera_game->down = camera_center_y - new_camera_half_height;
		camera_game->up = camera_center_y + new_camera_half_height;
	}
	//HANDLE camera move with right click resize.
	if (mouse_moved_this_frame && ui_state->mouse_right_click_down && (scene == SCENE_TYPE::ST_EDITOR || scene == SCENE_TYPE::ST_EDIT_LEVEL))
	{
		ui_state->totalMove = math_intpair_sub(ui_state->mouse_last_pos, ui_state->mousePos);
		camera_game->left += ui_state->totalMove.x * CAMERA_MOVE_MULT;
		camera_game->right += ui_state->totalMove.x * CAMERA_MOVE_MULT;
		camera_game->down += ui_state->totalMove.y * CAMERA_MOVE_MULT;
		camera_game->up += ui_state->totalMove.y * CAMERA_MOVE_MULT;
	}
}

#pragma region EMBARASSINGLY GLOBAL VARIABLES

bool keep_running_infinite_loop = false;
//Memory variables.
Memory* frame_memory;
Memory* permanent_memory;
Memory* menu_memory;
Memory* editor_memory;
Memory* play_memory;
Memory* world_memory;
Memory* level_memory;
Memory* animation_memory;
Memory* text_memory;
//main loop variables.
SCENE_TYPE scene;
EditorScene* editor_scene_state;
PlayScene play_scene_state;
WorldScene* world_scene_state;
WorldPlayScene* world_play_scene_state;
TextScene* text_scene_state;
MenuScene* menu_scene_state;
Direction last_move_taken = NO_DIRECTION;
int screen_width = 800;
int screen_height = 600;
float world_camera_lerp;
SDL_Event event;
bool running = true;
EditorUIState ui_state;

GameSpaceCamera world_camera;
GameSpaceCamera world_camera_start;
GameSpaceCamera world_camera_goal;

//camera stuff
GameSpaceCamera camera_game;
ViewPortCamera camera_viewport;
glm::mat4 camera;

//time machines.
//TimeMachineEditor* timeMachine;
//TimeMachineEditorStartState* time_machine_start_state;

//mouse state:
glm::vec2 dragging_start_position_in_gamespace = glm::vec2(0, 0);

//timers 
Uint32 start_time_ms;
Uint32 last_frame_time_ms;

//palete

//Shader nonsense
Shader spriteShader;
Shader fullSpriteShader;
Shader dottedShader;
Shader textShader;
Shader stringShader;

unsigned int floorAtlas;
unsigned int pieceAtlas;

LayerDrawGPUData* layer_draw;

GLuint floor_VAO;
GLuint floorPositionsBuffer;
GLuint floorAtlasBuffer;
glm::vec3* floor_positions_cpu;
glm::vec4* floor_atlas_cpu;
glm::vec4* floor_atlas_mapper;
int floor_total_drawn = 0;

GLuint piece_VAO;
GLuint piecePositionsBuffer;
GLuint pieceAtlasBuffer;
glm::vec4* piece_atlas_cpu;
glm::vec3* piece_positions_cpu;
glm::vec4* piece_atlas_mapper;
int piece_total_drawn = 0;

GLuint fullsprite_VAO;
GLuint fullspriteMatrixBuffer;
GLuint fullspriteAtlasBuffer;

GamefullspriteDrawInfo fullspriteDraw;

GLuint dotted_VAO;
GLuint dotted_positions_buffer;
GLuint dotted_scale_buffer;
int dotted_total_drawn;
glm::vec3* dotted_positions_cpu;
glm::vec2* dotted_scale_cpu;

GLuint string_VAO;
GLuint string_atlas_buffer;
GLuint string_matrix_buffer;
TextDrawInfo text_draw_info;

GamespriteDrawInfo gamespriteDrawInfo;

unsigned int string_texture;

SDL_Window* window;

#pragma endregion 

//these functions are also embarassingly global, they are going into our big class later.
void load_editor_level_stateful(std::string to_load)
{
	int bar = 3;
	{
		int bar = 4;
		bar++;
	}
	std::cout << to_load << std::endl;
	memory_clear(editor_memory);
	TimeMachineEditorStartState* res = parse_deserialize_timemachine(to_load, editor_memory, frame_memory);
	editor_scene_state = editorscene_setup_with_start_state(editor_memory, camera_viewport, res);
	editor_scene_state->timeMachine->current_number_of_actions = 0;
	std::cout << "ALL DONE!" << std::endl;
}
void resize_screen_stateful(int next_width, int next_height)
{
	float old_screen_width = (float) screen_width;
	float old_screen_height = (float) screen_height;
	float old_ratio = old_screen_width / old_screen_height;
	screen_width = next_width;
	screen_height = next_height;
	glViewport(0, 0, next_width, next_height);

	//GameSpaceCamera world_camera;
	//GameSpaceCamera world_camera_start;
	//GameSpaceCamera world_camera_goal;
	//GameSpaceCamera camera_game;
	camera_viewport.right = next_width;
	camera_viewport.up = next_height;

	//calculate the camera_game, where we just determine the new ratio, and then alter the width and height.
	{
		float next_ratio = (float) screen_width / screen_height;
		float expand_ratio = next_ratio / old_ratio;
		float center = (camera_game.left + camera_game.right) / 2.0f;
		float old_distance = camera_game.right - camera_game.left;
		float next_distance = old_distance * expand_ratio;
		camera_game.left = center - next_distance / 2.0f;
		camera_game.right = center + next_distance / 2.0f;
	}

	//calculate the next world scene camera... eh? lets do it later.
	//if we are in world scene mode, resize the world camera goal and snap the world camera start to that position as well.

}
void setup_world_screen_stateful()
{
	memory_clear(world_memory);
	world_scene_state = setup_world_scene(editor_scene_state->timeMachine, world_memory);
	scene = ST_PLAY_WORLD;
	ui_state.time_since_scene_started = 0;
	//setup world camera.
	//TODO: Compress this code and the world scene camera code into something better.
	int current_num = world_scene_state->current_level;
	GameState* current_gamestate = world_scene_state->level_state[current_num];
	IntPair gamestate_pos = world_scene_state->level_position[current_num];
	world_camera = math_camera_build_for_gamestate(current_gamestate, gamestate_pos, camera_viewport);
	world_camera_start = world_camera;
	world_camera_goal = world_camera;
	world_camera_lerp = CAMERA_LERP_TIME;
}

void menu_action_new_game()
{
	//TODO:
	//load using the default level name, the default world. into the world editor. (construct the world edit scene)
	//convert that default world to a world, then load the world scene.
}
void menu_action_continue_game()
{
	//TODO:
	//for now, we are just going to be exactly the same as new game. 
}
void menu_action_level_editor()
{
	//TODO: we open the level editor scene with an empty scene. Real easy.
}
/// we want to statefully calculate what action the player should take based upon two things.
/// 1. the editor UI, i.e. what buttons have been pressed recently.
/// 2. Action last_action_taken, i.e. what action did we last take?
Action calculate_what_action_to_take_next_stateful()
{
	//Direction last_move_taken = NO_DIRECTION;
	char letter_priority = ui_state.most_recently_pressed_direction;

	//calculate if we should perform a new move.
	{
		char button_names[4] = { 'w', 'a', 's', 'd' };
		Direction button_actions[4] = { U, L, D, R };
		bool button_pressed = false;
		int button_press_index = -1;
		float time_button_pressed = -1;
		for (int i = 0; i < 4; i++)
		{
			if (ui_state.letters[button_names[i] - 'a'].pressed)
				if (ui_state.letters[button_names[i] - 'a'].time_pressed > time_button_pressed || !button_pressed)
				{
					button_pressed = true;
					button_press_index = i;
					time_button_pressed = ui_state.letters[button_names[i] - 'a'].time_pressed;
				}
		}
		if (button_pressed)
		{
			if (ui_state.time_till_player_can_move <= 0 || button_actions[button_press_index] != last_move_taken)
			{
				last_move_taken = (Direction)button_actions[button_press_index];
				return (Action) button_actions[button_press_index];
			}
		}
	}
	if (ui_state.letters['z' - 'a'].pressed_this_frame ||
		(ui_state.letters['z' - 'a'].pressed && ui_state.time_till_player_can_move <= 0))
	{
		last_move_taken = Direction::NO_DIRECTION;
		return (Action) A_UNDO;

	}
	if (ui_state.letters['r' - 'a'].pressed_this_frame)
	{
		last_move_taken = Direction::NO_DIRECTION;
		return (Action)A_RESET;
	}
	return (Action) A_NONE;
}
void handle_next_action_stateful(GamestateTimeMachine* maybe_time_machine, IntPair draw_position, Animations** maybe_animation)
{
	Action next_action = calculate_what_action_to_take_next_stateful();

	Direction to_take = action_to_direction(next_action);

	if (to_take != NO_DIRECTION)
		take_player_action(maybe_animation,
			maybe_time_machine,
			draw_position,
			&ui_state,
			to_take,
			level_memory,
			frame_memory,
			animation_memory);


	if (next_action == A_UNDO)
	{
		gamestate_timemachine_undo(maybe_time_machine);
		ui_state.time_till_player_can_move = WAIT_BETWEEN_PLAYER_MOVE_REPEAT;
		*maybe_animation = NULL;
	}
	if (next_action == A_RESET)
	{
		gamestate_timemachine_reset(maybe_time_machine, level_memory);
		*maybe_animation = NULL;
	}
}
void mainloopfunction()
{
#pragma region Loop Startup
		//reset frame info
		{
			for (int i = 0; i < NUM_LETTERS_ON_KEYBOARD; i++)
			{
				ui_state.letters[i].pressed_this_frame = false;
				ui_state.letters[i].released_this_frame = false;
			}
			ui_state.update_actual_screen_size = false;
			ui_state.mouse_last_pos = ui_state.mousePos;
			ui_state.shift_key_down_this_frame = false;
			ui_state.click_left_down_this_frame = false;
			ui_state.click_left_up_this_frame = false;
			ui_state.wheel_move = 0;
			ui_state.totalMove = math_intpair_create(0, 0);
			ui_state.z_key_down_this_frame = false;
			ui_state.backspace_key_down_this_frame = false;
			ui_state.right.pressed_this_frame = false;
			ui_state.up.pressed_this_frame = false;
			ui_state.down.pressed_this_frame = false;
			ui_state.left.pressed_this_frame = false;
			ui_state.spacebar.pressed_this_frame = false;
			ui_state.enter.pressed_this_frame = false;
		}
		//poll events for tasty info.
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					ui_state.update_actual_screen_size = true;
					ui_state.next_camera_size = glm::ivec2(event.window.data1, event.window.data2);
				}
			}
			if (event.type == SDL_KEYDOWN)
			{
				for (int i = 0; i < NUM_LETTERS_ON_KEYBOARD; i++)
				{
					if (event.key.keysym.sym == (SDLK_a + i))	//first a, final z.
					{
						ui_state.letters[i].pressed = true;
						if (ui_state.letters[i].released_since_pressed_last)
						{
							ui_state.letters[i].pressed_this_frame = true;
							ui_state.letters[i].time_pressed = ui_state.total_time_passed;
						}
						ui_state.letters[i].released_since_pressed_last = false;
					}
				}
				if (event.key.keysym.sym == SDLK_LSHIFT)
				{
					ui_state.shift_key_down_this_frame = true;
					ui_state.shift_key_down = true;
				}
				if (event.key.keysym.sym == SDLK_LALT)
				{
					ui_state.alt_key_down = true;
				}
				if (event.key.keysym.sym == SDLK_LCTRL)
				{
					ui_state.control_key_down = true;
				}
				if (event.key.keysym.sym == SDLK_z)
				{
					ui_state.z_key_down_this_frame = true;
				}
				if (event.key.keysym.sym == SDLK_BACKSPACE)
				{
					ui_state.backspace_key_down_this_frame = true;
				}
				if (event.key.keysym.sym == SDLK_d)
				{
					ui_state.right.pressed = true;
					ui_state.right.pressed_this_frame = true;
				}
				if (event.key.keysym.sym == SDLK_w)
				{
					ui_state.up.pressed = true;
					ui_state.up.pressed_this_frame = true;
				}
				if (event.key.keysym.sym == SDLK_a)
				{
					ui_state.left.pressed = true;
					ui_state.left.pressed_this_frame = true;
				}
				if (event.key.keysym.sym == SDLK_s)
				{
					ui_state.down.pressed = true;
					ui_state.down.pressed_this_frame = true;
				}
				if (event.key.keysym.sym == SDLK_SPACE)
				{
					ui_state.spacebar.pressed = true;
					ui_state.spacebar.pressed_this_frame = true;
				}
				if (event.key.keysym.sym == SDLK_RETURN)
				{
					ui_state.enter.pressed = true;
					ui_state.enter.pressed_this_frame = true;
				}
			}
			if (event.type == SDL_KEYUP)
			{
				for (int i = 0; i < NUM_LETTERS_ON_KEYBOARD; i++)
				{
					if (event.key.keysym.sym == (SDLK_a + i))	//first a, final z.
					{
						ui_state.letters[i].pressed = false;
						ui_state.letters[i].released_this_frame = true;
						ui_state.letters[i].released_since_pressed_last = true;
					}
				}
				if (event.key.keysym.sym == SDLK_LSHIFT)
				{
					ui_state.shift_key_down = false;
				}
				if (event.key.keysym.sym == SDLK_LALT)
				{
					ui_state.alt_key_down = false;
				}
				if (event.key.keysym.sym == SDLK_LCTRL)
				{
					ui_state.control_key_down = false;
				}
				if (event.key.keysym.sym == SDLK_d)
				{
					ui_state.right.pressed = false;
				}
				if (event.key.keysym.sym == SDLK_w)
				{
					ui_state.up.pressed = false;
				}
				if (event.key.keysym.sym == SDLK_a)
				{
					ui_state.left.pressed = false;
				}
				if (event.key.keysym.sym == SDLK_s)
				{
					ui_state.down.pressed = false;
				}
				if (event.key.keysym.sym == SDLK_SPACE)
				{
					ui_state.spacebar.pressed = false;
				}
				if (event.key.keysym.sym == SDLK_RETURN)
				{
					ui_state.enter.pressed = false;
					ui_state.enter.released_this_frame = true;
					ui_state.enter.released_since_pressed_last = true;
				}
			}
			if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
			{
				//get the mouse coordinates and convert them to gamespace coordinates.
				{
					int mouseX;
					int mouseY;
					SDL_GetMouseState(&mouseX, &mouseY);
					mouseY = camera_viewport.up - mouseY;
					ui_state.mousePos = math_intpair_create(mouseX, mouseY);
					ui_state.mouseGamePos = math_screenspace_to_gamespace(ui_state.mousePos, camera_game, camera_viewport, ui_state.game_height_current);
				}

				//grab whether mouse was held down this frame or not.
				if (event.type == SDL_MOUSEBUTTONDOWN)
				{
					if (event.button.button == SDL_BUTTON_RIGHT)
						ui_state.mouse_right_click_down = true;
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						ui_state.click_left_down_this_frame = true;
						ui_state.mouse_left_click_down = true;
					}
				}
				//
				if (event.type == SDL_MOUSEBUTTONUP)
				{
					if (event.button.button == SDL_BUTTON_RIGHT)
						ui_state.mouse_right_click_down = false;
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						ui_state.click_left_up_this_frame = true;
						ui_state.mouse_left_click_down = false;
					}
				}
			}
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			if (event.type == SDL_MOUSEWHEEL)
			{
				if (event.wheel.y != 0)
				{
					ui_state.wheel_move = (float) event.wheel.y;
#ifdef EMSCRIPTEN
					//if we using emscripten, this wheel scroll value is 100 times the window version for some reason, so we divide it by 100.
					ui_state.wheel_move /= 100.0f;
#endif
				}
			}
		}
		//handle shared events.
		bool mouse_moved_this_frame = ui_state.mousePos.x != ui_state.mouse_last_pos.x || ui_state.mousePos.y != ui_state.mouse_last_pos.y;
		bool left_click_action_resolved = false;
		{
			HandleSharedEvents(&ui_state, &camera_game, &camera, mouse_moved_this_frame, scene);
			//HANDLE clicking on the palette.
			if (ui_state.click_left_down_this_frame && !ui_state.shift_key_down && !left_click_action_resolved && scene == SCENE_TYPE::ST_EDITOR)
			{
				glm::vec2 palete_gamespace_start = math_screenspace_to_gamespace(editor_scene_state->palete_screen_start, camera_game, camera_viewport, ui_state.game_height_current);
				bool clickedPalete = math_click_is_inside_AABB(
					palete_gamespace_start.x,
					palete_gamespace_start.y,
					palete_gamespace_start.x + palete_length,
					palete_gamespace_start.y + 1,
					ui_state.mouseGamePos.x,
					ui_state.mouseGamePos.y);
				if (clickedPalete)
				{
					//calculate what palete cell we actually clicked;
					float percentageX = percent_between_two_points(ui_state.mouseGamePos.x, palete_gamespace_start.x, palete_gamespace_start.x + palete_length);
					int palete_cell_clicked = (int)(percentageX * palete_length);
					editor_scene_state->currentBrush = palete_cell_clicked;
					//apply the brush.
					left_click_action_resolved = true;

				}
			}

		}
#pragma endregion
#pragma region GLClear And Update Delta Time
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//calculate delta
		float delta = 0;
		float total_time;
		{
			int current_time_ms = SDL_GetTicks();
			float delta_ms = (float) (current_time_ms - last_frame_time_ms);
			last_frame_time_ms = current_time_ms;
			delta = delta_ms / 1000.0f;
			total_time = (current_time_ms - start_time_ms) / 1000.0f;
		}
		ui_state.time_till_player_can_move = maxf(0, ui_state.time_till_player_can_move - delta);
		ui_state.time_since_last_player_action += delta;
		ui_state.time_since_scene_started += delta;
		ui_state.total_time_passed = total_time;

#pragma endregion

		if (scene == ST_EDITOR)
		{
#pragma region handle_events
			//now that we've polled all the events, we try and find out what we need to do.
			bool mouse_moved_this_frame = ui_state.mousePos.x != ui_state.mouse_last_pos.x || ui_state.mousePos.y != ui_state.mouse_last_pos.y;
			if (ui_state.update_actual_screen_size)
			{
				//rebuild the editor camera. 
				resize_screen_stateful(ui_state.next_camera_size.x, ui_state.next_camera_size.y);
			}
			if (ui_state.letters['y' - 'a'].pressed_this_frame)
			{
				SDL_SetWindowSize(window,1200, 900);
				resize_screen_stateful(1200, 900);

			}
			if (ui_state.type == ECS_BRUSH)
			{
				if (ui_state.mouse_left_click_down)
					MaybeApplyBrush(editor_scene_state->palete,editor_scene_state->currentBrush, &ui_state,editor_scene_state->timeMachine, ui_state.mouseGamePos);
				else
					ui_state.type = ECS_NEUTRAL;
			}
			if (ui_state.type == ECS_NEUTRAL)
			{
				//HANDLE entering game.
				if (ui_state.letters['m' - 'a'].pressed_this_frame)
				{
					setup_world_screen_stateful();
				}
				//HANDLE opening game file.
				if (ui_state.letters['o' - 'a'].pressed_this_frame)
				{
					std::string to_load = load_puzzle_file();
					if (to_load == "")
					{
						std::cout << "uh oh, our load of a game didn't work, nothings happened!";
					}
					else
					{
						load_editor_level_stateful(to_load);
					}
				}
				//HANDLE saving game file.
				if (ui_state.letters['p' - 'a'].pressed_this_frame)
				{

					std::string to_print = parse_serialize_timemachine(editor_scene_state->timeMachine, frame_memory, frame_memory);
					save_puzzle_file(to_print);
					std::cout << to_print << std::endl;
					//TimeMachineEditorStartState* res = parse_deserialize_timemachine(to_print, permanent_memory, frame_memory);
					//time_machine_start_state = res;
					//gamestate_timemachine_editor_initialise_from_start(timeMachine, time_machine_start_state);
					//timeMachine->current_number_of_actions = 0;

				}
				//HANDLE expanding width.
				if (ui_state.letters['w' - 'a'].pressed_this_frame)
				{
					int index_clicked = gamestate_timemachine_get_click_collision(editor_scene_state->timeMachine, ui_state.mouseGamePos.x, ui_state.mouseGamePos.y);
					if (index_clicked >= 0)
					{
						GameState* state = editor_scene_state->timeMachine->gamestates[index_clicked];
						IntPair state_start = editor_scene_state->timeMachine->gamestates_positions[index_clicked];
						IntPair grid_clicked = calculate_floor_cell_clicked(state, state_start, ui_state.mouseGamePos);
						//TODO: don't use permanent memory, use something else, just a bit easier to waste memory now.
						GameState* next = gamestate_add_row(state, editor_scene_state->timeMachine->gamestate_memory, grid_clicked.x, grid_clicked.y);
						TimeMachineEditorAction action = gamestate_timemachineaction_create_update_gamestate(next, index_clicked, &editor_scene_state->timeMachine->names[index_clicked * GAME_LEVEL_NAME_MAX_SIZE]);
						gamestate_timemachine_editor_take_action(editor_scene_state->timeMachine, NULL, action);
					}

				}
				//HANDLE exapnding height.
				if (ui_state.letters['h' - 'a'].pressed_this_frame)
				{
					int index_clicked = gamestate_timemachine_get_click_collision(editor_scene_state->timeMachine, ui_state.mouseGamePos.x, ui_state.mouseGamePos.y);
					if (index_clicked >= 0)
					{
						GameState* state = editor_scene_state->timeMachine->gamestates[index_clicked];
						IntPair state_start = editor_scene_state->timeMachine->gamestates_positions[index_clicked];
						IntPair grid_clicked = calculate_floor_cell_clicked(state, state_start, ui_state.mouseGamePos);
						//TODO: don't use permanent memory, use something else, just a bit easier to waste memory now.
						GameState* next = gamestate_add_column(state, permanent_memory, grid_clicked.x, grid_clicked.y);
						TimeMachineEditorAction action = gamestate_timemachineaction_create_update_gamestate(next, index_clicked, &editor_scene_state->timeMachine->names[index_clicked * GAME_LEVEL_NAME_MAX_SIZE]);
						gamestate_timemachine_editor_take_action(editor_scene_state->timeMachine, NULL, action);
					}
				}
				//HANDLE surrounding level with crates.
				if (ui_state.letters['c' - 'a'].pressed_this_frame)
				{
					int index_clicked = gamestate_timemachine_get_click_collision(editor_scene_state->timeMachine, ui_state.mouseGamePos.x, ui_state.mouseGamePos.y);
					if (index_clicked >= 0)
					{
						GameState* state = editor_scene_state->timeMachine->gamestates[index_clicked];
						GameState* next = gamestate_surround_with_walls(state, permanent_memory);
						TimeMachineEditorAction action = gamestate_timemachineaction_create_update_gamestate(next, index_clicked, &editor_scene_state->timeMachine->names[index_clicked * GAME_LEVEL_NAME_MAX_SIZE]);
						gamestate_timemachine_editor_take_action(editor_scene_state->timeMachine, NULL, action);
					}
				}
				//HANDLE DELETE GAMESTATE
				if (ui_state.click_left_down_this_frame && ui_state.control_key_down)
				{
					int index_clicked = gamestate_timemachine_get_click_collision(editor_scene_state->timeMachine, ui_state.mouseGamePos.x, ui_state.mouseGamePos.y);
					if (index_clicked >= 0)
					{
						GameState* currentState = editor_scene_state->timeMachine->gamestates[index_clicked];
						TimeMachineEditorAction action = gamestate_timemachineaction_create_delete_gamestate(index_clicked);
						gamestate_timemachine_editor_take_action(editor_scene_state->timeMachine, NULL, action);
						left_click_action_resolved = true;
					}
				}
				//HANDLE play mode clicking on a gamestate.
				if (ui_state.click_left_down_this_frame && ui_state.alt_key_down && !left_click_action_resolved)
				{
					int index_clicked = gamestate_timemachine_get_click_collision(editor_scene_state->timeMachine, ui_state.mouseGamePos.x, ui_state.mouseGamePos.y);
					if (index_clicked >= 0)
					{
						//get the gamestate we want. 
						GameState* start_state = editor_scene_state->timeMachine->gamestates[index_clicked];

						//create the play_scene_state scene based on that gamestate.
						play_scene_state.timeMachine = gamestate_timemachine_create(start_state, play_memory, max_actions_in_puzzle);
						play_scene_state.timeMachine_edit = gamestate_timemachine_create(start_state, play_memory, max_actions_in_puzzle);
						play_scene_state.loc = editor_scene_state->timeMachine->gamestates_positions[index_clicked];
						play_scene_state.loc_edit = play_scene_state.loc;
						play_scene_state.loc_edit.x += (start_state->w + 2);
						play_scene_state.editor_position_in_time_machine = index_clicked;
						for (int i = 0; i < GAME_LEVEL_NAME_MAX_SIZE; i++)
						{
							play_scene_state.game_name[i] = 0;
						}
						play_scene_state.game_name_length = 0;
						strcpy_s(play_scene_state.game_name, &editor_scene_state->timeMachine->names[index_clicked * GAME_LEVEL_NAME_MAX_SIZE]);
						play_scene_state.game_name_length = (int) strlen(&editor_scene_state->timeMachine->names[index_clicked * GAME_LEVEL_NAME_MAX_SIZE]);
						//switch our scene to that gamestate.
						scene = ST_EDIT_LEVEL;
						ui_state.time_since_scene_started = 0;
					}

				}
				//HANDLE clicking on a gamestate.
				if (ui_state.click_left_down_this_frame && !ui_state.shift_key_down && !left_click_action_resolved)
				{
					left_click_action_resolved = MaybeApplyBrush(editor_scene_state->palete, editor_scene_state->currentBrush, &ui_state, editor_scene_state->timeMachine, ui_state.mouseGamePos);
				}
				//HANDLE clicking on a gamestates edge.
				if (ui_state.mouse_left_click_down && !ui_state.shift_key_down && !left_click_action_resolved)
				{
					//determine if we are clicking on a gamestate border.
					for (int i = 0; i < editor_scene_state->timeMachine->current_number_of_gamestates; i++)
					{
						float box_left = (float)editor_scene_state->timeMachine->gamestates_positions[i].x;
						float box_width = (float)editor_scene_state->timeMachine->gamestates[i]->w;
						float box_right = box_left + box_width;
						float box_down = (float)editor_scene_state->timeMachine->gamestates_positions[i].y;
						float box_height = (float)editor_scene_state->timeMachine->gamestates[i]->h;
						float box_up = box_down + box_height;
						AABB left = math_AABB_create(box_left - OUTLINE_DRAW_SIZE,
							box_down - OUTLINE_DRAW_SIZE,
							OUTLINE_DRAW_SIZE,
							box_height + OUTLINE_DRAW_SIZE * 2);
						AABB right = math_AABB_create(box_right,
							box_down - OUTLINE_DRAW_SIZE,
							OUTLINE_DRAW_SIZE,
							box_height + OUTLINE_DRAW_SIZE * 2);
						AABB up = math_AABB_create(box_left - OUTLINE_DRAW_SIZE,
							box_up,
							box_width + OUTLINE_DRAW_SIZE * 2,
							OUTLINE_DRAW_SIZE);
						AABB down = math_AABB_create(box_left - OUTLINE_DRAW_SIZE,
							box_down - OUTLINE_DRAW_SIZE,
							box_width + OUTLINE_DRAW_SIZE * 2,
							OUTLINE_DRAW_SIZE);

						bool click_left = math_click_is_inside_AABB(ui_state.mouseGamePos.x, ui_state.mouseGamePos.y, left);
						bool click_right = math_click_is_inside_AABB(ui_state.mouseGamePos.x, ui_state.mouseGamePos.y, right);
						bool click_up = math_click_is_inside_AABB(ui_state.mouseGamePos.x, ui_state.mouseGamePos.y, up);
						bool click_down = math_click_is_inside_AABB(ui_state.mouseGamePos.x, ui_state.mouseGamePos.y, down);
						if (click_left || click_right || click_up || click_down)
						{
							ui_state.type = ECS_RESIZE_GAMESTATE;
							ui_state.un.resize.dragging_left = false;
							ui_state.un.resize.dragging_right = false;
							ui_state.un.resize.dragging_up = false;
							ui_state.un.resize.dragging_down = false;
							if (click_left)
								ui_state.un.resize.dragging_left = true;
							if (click_right)
								ui_state.un.resize.dragging_right = true;
							if (click_up)
								ui_state.un.resize.dragging_up = true;
							if (click_down)
								ui_state.un.resize.dragging_down = true;

							ui_state.un.resize.dragging_gamestate_index = i;
							left_click_action_resolved = true;
							dragging_start_position_in_gamespace = ui_state.mouseGamePos;
						}
					}
				}
				//HANDLE shift clicking on a gamestate
				if (ui_state.mouse_left_click_down && ui_state.shift_key_down)
				{
					int index_clicked = gamestate_timemachine_get_click_collision(editor_scene_state->timeMachine, ui_state.mouseGamePos.x, ui_state.mouseGamePos.y);
					if (index_clicked >= 0)
					{
						ui_state.type = ECS_MOVE_GAMESTATE;
						ui_state.un.move.moving_gamestate_index = index_clicked;
						ui_state.un.move.move_start_position = ui_state.mouseGamePos;
						left_click_action_resolved = true;
					}
				}
				//HANDLE clicking on an empty square in the game.
				if (ui_state.click_left_down_this_frame && !ui_state.shift_key_down && !left_click_action_resolved)
				{
					//determine what "game world" square we are clicking on.
					IntPair game_world_click = math_intpair_create((int) floor(ui_state.mouseGamePos.x),(int) floor(ui_state.mouseGamePos.y));
					ui_state.type = ECS_CREATE_GAMESTATE;
					ui_state.un.create.gameworld_start_pos = game_world_click;
				}
				if (ui_state.z_key_down_this_frame && !left_click_action_resolved)
				{
					TimeMachineEditorAction action;
					action.action = TM_UNDO;
					action.u.remove.gamestate_index = -1;
					gamestate_timemachine_editor_take_action(editor_scene_state->timeMachine, editor_scene_state->time_machine_start_state, action);
					left_click_action_resolved = true;
				}

			}
			if (ui_state.type == ECS_RESIZE_GAMESTATE)
			{
				//handle stopping 
				if (ui_state.click_left_up_this_frame)
				{
					//determine (using ints to round down) how far away from the starting position we are.
					glm::vec2 offset = ui_state.mouseGamePos - dragging_start_position_in_gamespace;
					//get the width and height of the targetted gamestate.
					IntPair targetted_gamestate_position = editor_scene_state->timeMachine->gamestates_positions[ui_state.un.resize.dragging_gamestate_index];
					GameState* targetted_gamestate = editor_scene_state->timeMachine->gamestates[ui_state.un.resize.dragging_gamestate_index];
					int targetted_gamestate_width = targetted_gamestate->w;
					int targetted_gamestate_height = targetted_gamestate->h;

					AABB old = math_AABB_create((float) targetted_gamestate_position.x,
						(float) targetted_gamestate_position.y,
						(float) targetted_gamestate_width,
						(float) targetted_gamestate_height);
					AABB next = old;

					//IntPair next_start = targetted_gamestate_position;
					//int next_w = targetted_gamestate->w;
					//int next_h = targetted_gamestate->h;

					float offset_x = sign(offset.x) * 0.5f;
					float offset_y = sign(offset.y) * 0.5f;
					if (ui_state.un.resize.dragging_left)
					{
						int left_movement = (int)(ui_state.mouseGamePos.x - ((float)next.x) + offset_x);
						if (left_movement != 0)
						{
							next.x += left_movement;
							next.w -= left_movement;
						}
					}
					if (ui_state.un.resize.dragging_right)
					{
						int right_movement = (int)(ui_state.mouseGamePos.x - ((float)next.x + next.w) + offset_x);
						if (right_movement != 0)
							next.w += right_movement;
					}
					if (ui_state.un.resize.dragging_down)
					{
						int down_movement = (int)(ui_state.mouseGamePos.y - ((float)next.y) + offset_y);
						if (down_movement != 0)
						{
							next.y += down_movement;
							next.h -= down_movement;
						}
					}
					if (ui_state.un.resize.dragging_up)
					{
						int up_movement = (int)(ui_state.mouseGamePos.y - ((float)next.y + next.h) + offset_y);
						if (up_movement != 0)
							next.h += up_movement;
					}

					bool changeOccured = next.x != targetted_gamestate_position.x
						|| next.y != targetted_gamestate_position.y
						|| next.w != targetted_gamestate->w
						|| next.h != targetted_gamestate->h;

					//handle the edge case where we resize too far and the gameobject would be destroyed.
					if (next.w <= 0)
					{
						int go_back = (int) next.w - 1;
						next.w = 1;
						if (ui_state.un.resize.dragging_left)
						{
							next.x += go_back;
						}
					}

					if (next.h <= 0)
					{
						int go_back = (int) next.h - 1;
						next.h = 1;
						if (ui_state.un.resize.dragging_down)
						{
							next.y += go_back;
						}
					}


					if (changeOccured)
					{
						AABB* boxes = gamestate_create_colliders(frame_memory,
							editor_scene_state->timeMachine->gamestates,
							editor_scene_state->timeMachine->gamestates_positions,
							editor_scene_state->timeMachine->current_number_of_gamestates,
							ui_state.un.resize.dragging_gamestate_index);
						bool changeValid = !math_AABB_is_colliding(next, boxes, editor_scene_state->timeMachine->current_number_of_gamestates - 1);
						if (changeValid)
						{
							IntPair next_size;
							next_size.x = (int) next.w;
							next_size.y = (int) next.h;
							IntPair displacement;
							displacement.x = (int) (-next.x + targetted_gamestate_position.x);
							displacement.y = (int) (-next.y + targetted_gamestate_position.y);
							TimeMachineEditorAction action =
								gamestate_timemachineaction_create_resize_gamsestate(ui_state.un.resize.dragging_gamestate_index, next, displacement);
							gamestate_timemachine_editor_take_action(editor_scene_state->timeMachine, NULL, action);
						}

					}

					ui_state.type = ECS_NEUTRAL;
				}
			}
			if (ui_state.type == ECS_MOVE_GAMESTATE)
			{
				if (ui_state.click_left_up_this_frame)
				{
					//calculate the space we are currently under
					AABB next = calculate_outline_from_move_info(frame_memory, editor_scene_state->timeMachine, ui_state);
					AABB* boxes = gamestate_create_colliders(frame_memory,
						editor_scene_state->timeMachine->gamestates,
						editor_scene_state->timeMachine->gamestates_positions,
						editor_scene_state->timeMachine->current_number_of_gamestates,
						ui_state.un.move.moving_gamestate_index);
					bool changeValid = !math_AABB_is_colliding(next, boxes, editor_scene_state->timeMachine->current_number_of_gamestates - 1);
					if (changeValid)
					{
						//calculate the distance between that space and the starting space.
						glm::vec2 distance = ui_state.mouseGamePos - ui_state.un.move.move_start_position;
						//transfer that into an int offset to apply to the moved gamestate.
						IntPair offset = math_intpair_create((int)distance.x, (int)distance.y);
						//create a move action, and apply it to the timeMachine.
						TimeMachineEditorAction action = gamestate_timemachineaction_create_move_gamestate(ui_state.un.move.moving_gamestate_index, offset);
						gamestate_timemachine_editor_take_action(editor_scene_state->timeMachine, NULL, action);
					}


					//revert the ui_state to neutral.
					ui_state.type = ECS_NEUTRAL;
				}
			}
			if (ui_state.type == ECS_CREATE_GAMESTATE && !ui_state.click_left_down_this_frame)
			{

				if (ui_state.click_left_up_this_frame)
				{
					//calculate the space we are currently under
					AABB next = calculate_outline_from_create_info(frame_memory, editor_scene_state->timeMachine, ui_state);
					AABB* boxes = gamestate_create_colliders(frame_memory,
						editor_scene_state->timeMachine->gamestates,
						editor_scene_state->timeMachine->gamestates_positions,
						editor_scene_state->timeMachine->current_number_of_gamestates);
					bool successful_plant = !math_AABB_is_colliding(next, boxes, editor_scene_state->timeMachine->current_number_of_gamestates);
					if (successful_plant)
					{
						AABB current_outline = calculate_outline_from_create_info(frame_memory, editor_scene_state->timeMachine, ui_state);
						TimeMachineEditorAction action = gamestate_timemachineaction_create_create_action((int) current_outline.x, (int) current_outline.y, (int) current_outline.w, (int) current_outline.h);
						gamestate_timemachine_editor_take_action(editor_scene_state->timeMachine, NULL, action);
					}
					ui_state.type = ECS_NEUTRAL;
				}


			}
#pragma endregion

		}
		else if (scene == ST_EDIT_LEVEL)
		{
#pragma region handle_events
			if (ui_state.type == ECS_BRUSH)
			{
				if (ui_state.mouse_left_click_down)
					bool left_click_action_resolved = MaybeApplyBrushInPlayMode(play_memory, editor_scene_state->palete, editor_scene_state->currentBrush, &ui_state, play_scene_state.timeMachine_edit, play_scene_state.loc_edit, ui_state.mouseGamePos);
				else
					ui_state.type = ECS_NEUTRAL;
			}
			else if (ui_state.type == ECS_NEUTRAL)
			{
				Animations* maybe_animation = NULL;
				Animations** maybe_animation_ptr = &maybe_animation;
				handle_next_action_stateful(play_scene_state.timeMachine, play_scene_state.loc, maybe_animation_ptr);
				if (ui_state.spacebar.pressed_this_frame)
				{
					GameState* current_edit_state = &play_scene_state.timeMachine_edit->state_array[play_scene_state.timeMachine_edit->num_gamestates_stored - 1];
					play_scene_state.timeMachine = gamestate_timemachine_create(current_edit_state, play_memory, MAX_NUMBER_GAMESTATES);

				}
				if (ui_state.mouse_left_click_down)
				{
					bool left_click_action_resolved = MaybeApplyBrushInPlayMode(play_memory, editor_scene_state->palete, editor_scene_state->currentBrush, &ui_state, play_scene_state.timeMachine_edit, play_scene_state.loc_edit, ui_state.mouseGamePos);
					if (left_click_action_resolved)
						ui_state.type = ECS_BRUSH;
				}
				if (ui_state.backspace_key_down_this_frame)
				{
					scene = ST_EDITOR;
					ui_state.time_since_scene_started = 0;
					int num_states = play_scene_state.timeMachine_edit->num_gamestates_stored;
					GameState* state_to_insert = gamestate_clone(&play_scene_state.timeMachine_edit->state_array[num_states - 1], permanent_memory);
					TimeMachineEditorAction action = gamestate_timemachineaction_create_replace_gamestate(state_to_insert, play_scene_state.editor_position_in_time_machine, play_scene_state.game_name);
					gamestate_timemachine_editor_take_action(editor_scene_state->timeMachine, NULL, action);
				}
				if (ui_state.shift_key_down_this_frame)
				{
					ui_state.type = ECS_EDIT_NAME;
				}
				if (ui_state.z_key_down_this_frame)
				{
					gamestate_timemachine_undo(play_scene_state.timeMachine);
				}
			}
			else if (ui_state.type == ECS_EDIT_NAME)
			{
				if (play_scene_state.game_name_length < GAME_LEVEL_NAME_MAX_SIZE)
				{
					for (int i = 0; i < NUM_LETTERS_ON_KEYBOARD - 1; i++)
					{
						if (ui_state.letters[i].pressed_this_frame)
						{
							//TODO: Slam down the text into our gamestate string!
							int length = play_scene_state.game_name_length;
							play_scene_state.game_name[length] = ('a' + (char) i);
							play_scene_state.game_name[length + 1] = 0;
							play_scene_state.game_name_length++;
						}
					}
					if (ui_state.spacebar.pressed_this_frame)
					{
						int length = play_scene_state.game_name_length;
						play_scene_state.game_name[length] = ' ';
						play_scene_state.game_name[length + 1] = 0;
						play_scene_state.game_name_length++;
					}
				}
				if (play_scene_state.game_name_length > 0)
				{
					if (ui_state.backspace_key_down_this_frame)
					{
						int length = play_scene_state.game_name_length;
						play_scene_state.game_name[length - 1] = 0;
						play_scene_state.game_name_length--;
					}
				}
				if (ui_state.enter.pressed_this_frame)
				{
					ui_state.type = ECS_NEUTRAL;
				}

			}
#pragma endregion

		}
		else if (scene == ST_PLAY_WORLD)
		{
#pragma region save state info before events
			int old_gamestate_num_player_standing_on = world_scene_state->current_level;
#pragma endregion
#pragma region handle_events
			bool world_action_taken = false;
			Action action = calculate_what_action_to_take_next_stateful();
			Direction to_take = action_to_direction(action);
			if (to_take != Direction::NO_DIRECTION)
			{
				world_play_scene_state = world_player_action(world_scene_state, to_take, level_memory);
				ui_state.time_till_player_can_move = WAIT_BETWEEN_PLAYER_MOVE_REPEAT;
				ui_state.time_since_last_player_action = 0;
			}
			if (ui_state.backspace_key_down_this_frame)
			{
				scene = ST_EDITOR;
				ui_state.time_since_scene_started = 0;
				ui_state.time_since_last_player_action = 0;
			}

			//after taking an action, if there's suddenly a time machine, that means its time to switch scenes!
			if (world_play_scene_state != NULL)
			{
				//scene = SCENE_TYPE::ST_PLAY_LEVEL;
				scene = SCENE_TYPE::ST_SHOW_TEXT;
				ui_state.time_since_scene_started = 0;
				ui_state.time_since_last_player_action = 0;
				text_scene_state = level_popup(&world_scene_state->level_names[world_scene_state->current_level * GAME_LEVEL_NAME_MAX_SIZE], text_memory, ui_state.total_time_passed);
			}

#pragma endregion	
#pragma region handle_state_update
			//handle camera
			{
				int current_num_player_standing_on = world_scene_state->current_level;
				if (old_gamestate_num_player_standing_on != current_num_player_standing_on)
				{
					world_camera_lerp = 0;
					GameState* current_gamestate = world_scene_state->level_state[current_num_player_standing_on];
					IntPair gamestate_pos = world_scene_state->level_position[current_num_player_standing_on];
					world_camera_goal = math_camera_build_for_gamestate(current_gamestate, gamestate_pos, camera_viewport);
					world_camera_start = world_camera;
				}
				else
				{
					world_camera_lerp += delta;
				}
				world_camera = math_camera_move_towards_lerp(world_camera_start, world_camera_goal, world_camera_lerp, CAMERA_LERP_TIME);
			}
#pragma endregion

		}
		else if (scene == ST_PLAY_LEVEL)
		{
#pragma region save state info before events
			int old_gamestate_num_player_standing_on = world_scene_state->current_level;
#pragma endregion 
#pragma region handle events
			//if we should be allowed to take actions:
				handle_next_action_stateful(world_play_scene_state->time_machine,world_play_scene_state->draw_position, &world_play_scene_state->maybe_animation);

#pragma endregion
#pragma region handle_state_update
			//handle camera
			{
				int current_num_player_standing_on = world_scene_state->current_level;
				if (old_gamestate_num_player_standing_on != current_num_player_standing_on)
				{
					world_camera_lerp = 0;
				}
				else
				{
					world_camera_lerp += delta;
				}
				GameState* current_gamestate = world_scene_state->level_state[current_num_player_standing_on];
				IntPair gamestate_pos = world_scene_state->level_position[current_num_player_standing_on];
				world_camera_goal = math_camera_build_for_gamestate(current_gamestate, gamestate_pos, camera_viewport, 0.3f, 0.3f);
				world_camera_start = world_camera;
				//world_camera_goal
				//world_camera_start
				//world_camera
				//camera
			}
			world_camera = math_camera_move_towards_lerp(world_camera_start, world_camera_goal, world_camera_lerp, CAMERA_LERP_TIME);
			//camera = camera_make_matrix(world_camera);
			//handle win update.
			if (gamestate_is_in_win_condition(&world_play_scene_state->time_machine->state_array[world_play_scene_state->time_machine->num_gamestates_stored - 1]))
			{
				world_scene_state->level_solved[world_scene_state->current_level] = true;
				int num_states = world_play_scene_state->time_machine->num_gamestates_stored;
				GameState* current_state = &world_play_scene_state->time_machine->state_array[num_states - 1];
				GameState* cloned_state = gamestate_clone(current_state, world_memory);
				gamestate_crumble(cloned_state);
				world_scene_state->level_state[world_scene_state->current_level] = cloned_state;
				world_play_scene_state->time_machine = NULL;
			}
			//handle returning to world map by request.
			if (ui_state.backspace_key_down_this_frame)
			{
				world_play_scene_state->time_machine = NULL;
			}
			//handle returning to world map by level finished.
			if (!world_play_scene_state->time_machine)
			{
				scene = SCENE_TYPE::ST_PLAY_WORLD;
				world_play_scene_state = NULL;
				ui_state.time_since_scene_started = 0;
			}
#pragma endregion
		}
		else if (scene == ST_SHOW_TEXT)
		{
#pragma region update
			if (text_scene_state->end_time <= ui_state.total_time_passed)
			{
				scene = text_scene_state->scene_to_revert_to;
				ui_state.time_since_scene_started = 0;
				ui_state.time_since_last_player_action = 0;
			}
#pragma endregion
#pragma region handle_events
		//TODO: if the player presses the action key (x, for now), the scene immediately ends. if the player presses 
		if (ui_state.letters['x' - 'a'].pressed_this_frame)
		{
			scene = text_scene_state->scene_to_revert_to;
			ui_state.time_since_scene_started = 0;
			ui_state.time_since_last_player_action = 0;
			//TODO.
		}
#pragma endregion
		}
		else if (scene == ST_MENU)
		{
#pragma region handle_events
		//handle the player pressing up arrow // 'w', or down arrow // 's', which changes menu.
		if (ui_state.letters['w' - 'a'].pressed_this_frame)
		{
			menu_scene_state->current_highlighted_button += 1;
			menu_scene_state->current_highlighted_button = mini(menu_scene_state->current_highlighted_button, menu_scene_state->num_buttons);
		
		}
		//handle the player pressing enter, or the action key 'x', which routes the current buttons callback to 
		if (ui_state.letters['s' - 'a'].pressed_this_frame)
		{
			menu_scene_state->current_highlighted_button -= 1;
			menu_scene_state->current_highlighted_button = maxi(menu_scene_state->current_highlighted_button, 0);
		}
		if (ui_state.letters['x' - 'a'].pressed_this_frame)
		{
			int current_button = menu_scene_state->current_highlighted_button;
			menu_scene_state->buttons[current_button].callback();
		}
#pragma endregion
		}

		if (scene == ST_EDITOR)
		{
#pragma region send draw data to gpu
			//parse gamestate outlines
			int skip_index = -1;
			AABB outline = math_AABB_create(0, 0, 0, 0);
			{
				if (ui_state.type == ECS_RESIZE_GAMESTATE)
				{
					skip_index = ui_state.un.resize.dragging_gamestate_index;
					outline = calculate_outline_position_from_drag_info(frame_memory, editor_scene_state->timeMachine, ui_state, dragging_start_position_in_gamespace);
				}
				if (ui_state.type == ECS_MOVE_GAMESTATE)
				{
					outline = calculate_outline_from_move_info(frame_memory, editor_scene_state->timeMachine, ui_state);
					skip_index = ui_state.un.move.moving_gamestate_index;
				}
				if (ui_state.type == ECS_CREATE_GAMESTATE)
				{
					outline = calculate_outline_from_create_info(frame_memory, editor_scene_state->timeMachine, ui_state);
				}
				if (skip_index >= 0 || ui_state.type == ECS_CREATE_GAMESTATE)
				{
					outline.x -= 0.1f;
					outline.y -= 0.1f;
					outline.w += 0.2f;
					outline.h += 0.2f;
					AABB* outlines_broken = (AABB*)memory_alloc(frame_memory, sizeof(AABB) * 4);
					math_AABB_break_into_borders(outline, outlines_broken, 0.2f);
					draw_outline_to_gamespace(outlines_broken[0], &fullspriteDraw);
					draw_outline_to_gamespace(outlines_broken[1], &fullspriteDraw);
					draw_outline_to_gamespace(outlines_broken[2], &fullspriteDraw);
					draw_outline_to_gamespace(outlines_broken[3], &fullspriteDraw);
				}
			}
			//parse gamestate outlines.
			{
				draw_gamestates_outlines_to_gamespace(
					editor_scene_state->timeMachine->gamestates,
					editor_scene_state->timeMachine->gamestates_positions,
					editor_scene_state->timeMachine->current_number_of_gamestates,
					&fullspriteDraw,
					skip_index);
			}
			//parse outline floor data.
			if (ui_state.game_height_current <= MAX_ZOOM_OUT_TO_STILL_DISPLAY_OUTLINES)
			{
				int outline_height = (int)ui_state.game_height_current + 3;
				int outline_width = (int)(SCREEN_RATIO * outline_height) + 1;
				IntPair bottom_left_screenspace = math_intpair_create(0, 0);
				glm::vec2 bottom_left_gamespace = math_screenspace_to_gamespace(bottom_left_screenspace, camera_game, camera_viewport, ui_state.game_height_current);
				glm::vec2 outlines_start_position = glm::vec2(bottom_left_gamespace.x - 1, bottom_left_gamespace.y - 1);
				outlines_start_position = glm::vec2((int)outlines_start_position.x, (int)outlines_start_position.y);
				for (int i = 0; i < outline_width; i++)
					for (int j = 0; j < outline_height; j++)
					{
						int index = f2D(i, j, outline_width, outline_height);
						Floor f = F_OUTLINE;
						floor_atlas_cpu[floor_total_drawn + index] = floor_atlas_mapper[f];
						floor_positions_cpu[floor_total_drawn + index] = glm::vec3(outlines_start_position.x + i, outlines_start_position.y + j, 0);
					}
				floor_total_drawn += outline_width * outline_height;
			}
			//parse gamestates
			{
				draw_layers_to_gamespace(
					editor_scene_state->timeMachine->gamestates,
					editor_scene_state->timeMachine->gamestates_positions,
					editor_scene_state->timeMachine->current_number_of_gamestates,
					layer_draw);

				draw_curse_to_gamespace(editor_scene_state->timeMachine->gamestates,
					editor_scene_state->timeMachine->gamestates_positions,
					editor_scene_state->timeMachine->current_number_of_gamestates,
					layer_draw);
			}
			//parse palette data to gpu form
			draw_palette(editor_scene_state->palete_screen_start, camera_game, camera_viewport, &ui_state, palete_length, editor_scene_state->palete, layer_draw);
			//parse dotted data to gpu form.
			{
				for (int z = 0; z < skip_index; z++)
				{
					int w = editor_scene_state->timeMachine->gamestates[z]->w;
					int h = editor_scene_state->timeMachine->gamestates[z]->h;
					IntPair startPos = editor_scene_state->timeMachine->gamestates_positions[z];
					dotted_positions_cpu[dotted_total_drawn] = glm::vec3(startPos.x - 0.15f, startPos.y - 0.15f, 8);
					dotted_scale_cpu[dotted_total_drawn] = glm::vec2(w + 0.30f, h + 0.30f);
					dotted_total_drawn++;
				}
				for (int z = skip_index + 1; z < editor_scene_state->timeMachine->current_number_of_gamestates; z++)
				{
					int w = editor_scene_state->timeMachine->gamestates[z]->w;
					int h = editor_scene_state->timeMachine->gamestates[z]->h;
					IntPair startPos = editor_scene_state->timeMachine->gamestates_positions[z];
					dotted_positions_cpu[dotted_total_drawn] = glm::vec3(startPos.x - 0.15f, startPos.y - 0.15f, 8);
					dotted_scale_cpu[dotted_total_drawn] = glm::vec2(w + 0.30f, h + 0.30f);
					dotted_total_drawn++;
				}
			}
			//draw text.
			{
				//draw_text_to_screen(glm::vec3(0, 0, 0), "FINALLY!", text_draw_info.string_matrix_cpu, text_draw_info.string_atlas_cpu, text_draw_info.true_font_reference, text_draw_info.text_positions, text_draw_info.text_positions_normalized, &string_total_drawn, SCREEN_STARTING_HEIGHT / ui_state.game_height_current);
				int len = editor_scene_state->timeMachine->current_number_of_gamestates;
				for (int i = 0; i < len; i++)
				{
					char* name = &editor_scene_state->timeMachine->names[i * GAME_LEVEL_NAME_MAX_SIZE];
					float x_pos = (float)editor_scene_state->timeMachine->gamestates_positions[i].x;
					float y_pos = (float)(editor_scene_state->timeMachine->gamestates_positions[i].y + editor_scene_state->timeMachine->gamestates[i]->h) + 0.2f;
					glm::vec3 draw_pos = glm::vec3(x_pos, y_pos, 0);
					//draw_text_to_screen(draw_pos, name, string_matrix_cpu, string_atlas_cpu, string_true_font_reference, text_positions, text_positions_normalized, &string_total_drawn, SCREEN_STARTING_HEIGHT / ui_state.game_height_current);
					draw_text_to_screen(draw_pos, glm::vec2(1, 1), name, &text_draw_info);
				}
			}
#pragma endregion
		}
		if (scene == ST_EDIT_LEVEL)
		{
#pragma region send draw data to gpu
			{
				draw_palette(editor_scene_state->palete_screen_start, camera_game, camera_viewport, &ui_state, palete_length, editor_scene_state->palete, layer_draw);

				GameState* play_draw = &play_scene_state.timeMachine->state_array[play_scene_state.timeMachine->num_gamestates_stored - 1];
				GameState* edit_draw = &play_scene_state.timeMachine_edit->state_array[play_scene_state.timeMachine_edit->num_gamestates_stored - 1];
				IntPair start_position = play_scene_state.loc;
				IntPair next_position = play_scene_state.loc_edit;
				draw_layers_to_gamespace(
					&(play_draw),
					&(play_scene_state.loc),
					1,
					layer_draw);
				draw_curse_to_gamespace(&(play_draw),
					&(play_scene_state.loc),
					1,
					layer_draw);
				draw_layers_to_gamespace(
					&(edit_draw),
					&(next_position),
					1,
					layer_draw);
				draw_curse_to_gamespace(&(edit_draw),
					&(next_position),
					1,
					layer_draw);
				glm::vec3 text_start_pos = glm::vec3(next_position.x, next_position.y + edit_draw->h, 0);
				draw_text_to_screen(text_start_pos, glm::vec2(1, 1), play_scene_state.game_name, &text_draw_info);
				//draw_text_to_screen(text_start_pos, play_scene_state.game_name, string_matrix_cpu, string_atlas_cpu, string_true_font_reference, text_positions, text_positions_normalized, &string_total_drawn, SCREEN_STARTING_HEIGHT / ui_state.game_height_current);
			}
#pragma endregion
		}
		if (scene == ST_PLAY_WORLD)
		{
#pragma region send draw data to gpu
			draw_layers_to_gamespace(
				world_scene_state->level_state,
				world_scene_state->level_position,
				world_scene_state->num_levels,
				layer_draw);
			draw_curse_to_gamespace(
				world_scene_state->level_state,
				world_scene_state->level_position,
				world_scene_state->num_levels,
				layer_draw);
#pragma endregion
		}
		if (scene == ST_PLAY_LEVEL)
		{
#pragma region send draw data to gpu
			int world_index_to_draw = world_scene_state->current_level;
			//draw the static gamestate, or draw the gamestate animation.
			{
				int gamestate_index_to_draw = world_play_scene_state->time_machine->num_gamestates_stored;
				GameState* to_draw = &world_play_scene_state->time_machine->state_array[gamestate_index_to_draw - 1];
				int current_level = world_scene_state->current_level;
				IntPair* to_draw_position = &world_play_scene_state->draw_position;
				//IntPair* to_draw_position = &world_scene_state->level_position[current_level];
				bool can_draw_movement = world_play_scene_state->maybe_animation && world_play_scene_state->maybe_animation->maybe_movement_animation;
				bool can_draw_curse = world_play_scene_state->maybe_animation && world_play_scene_state->maybe_animation->curse_animation;
				if (can_draw_movement)
				{
					draw_layer_to_gamespace(&to_draw, to_draw_position, 1, layer_draw, LN_FLOOR);
					bool done_animating_movement = true;
					done_animating_movement = draw_animation_to_gamespace(world_play_scene_state->maybe_animation->maybe_movement_animation, layer_draw, LN_PIECE, ui_state.time_since_last_player_action);
					if (done_animating_movement)
					{
						draw_piece_curses_to_gamespace(&to_draw, to_draw_position, 1, layer_draw, LN_PIECE, world_play_scene_state->maybe_animation->curse_animation, ui_state.time_since_last_player_action);
					}
					else
					{
						draw_stationary_piece_curses_to_gamespace(world_play_scene_state->maybe_animation->maybe_movement_animation,
							world_play_scene_state->maybe_animation->curse_animation,
							&world_play_scene_state->maybe_animation->old_state,
							to_draw_position,
							1,
							layer_draw,
							LN_PIECE,
							ui_state.time_since_last_player_action);
					}
				}
				else
				{
					draw_layers_to_gamespace(
						&to_draw,
						to_draw_position,
						1,
						layer_draw);
				}
				if (!can_draw_curse)
				{
					draw_curse_to_gamespace(&to_draw, to_draw_position, 1, layer_draw);
				}
				else if (can_draw_curse && !can_draw_movement)
				{
					draw_stationary_piece_curses_to_gamespace(world_play_scene_state->maybe_animation->maybe_movement_animation,
						world_play_scene_state->maybe_animation->curse_animation,
						&world_play_scene_state->maybe_animation->old_state,
						to_draw_position,
						1,
						layer_draw,
						LN_PIECE,
						ui_state.time_since_last_player_action);
				}

			}
#pragma endregion
		}
		if (scene == ST_SHOW_TEXT)
		{
#pragma region send draw data to gpu    
			draw_text_maximized_centered_to_screen(world_camera, text_scene_state->to_display, &text_draw_info);
			draw_black_box_over_screen(world_camera, &fullspriteDraw);
#pragma endregion
		}
		if (scene == ST_MENU)
		{
#pragma region send draw data to gpu
		//draw the first text
			for (int i = 0; i < menu_scene_state->num_buttons; i++)
			{
				//TODO: actually draw.
			}
#pragma endregion
		}
#pragma region draw gpu data

		//update camera.
		{
			if (scene == ST_PLAY_WORLD || scene == ST_PLAY_LEVEL || scene == ST_SHOW_TEXT)
				camera = camera_make_matrix(world_camera);
			if (scene == ST_EDITOR || scene == ST_EDIT_LEVEL)
				camera = camera_make_matrix(camera_game);
			glUseProgram(spriteShader);
			shader_set_uniform_mat4(spriteShader, "viewProjectionMatrix", camera);
			glUseProgram(fullSpriteShader);
			shader_set_uniform_mat4(fullSpriteShader, "viewProjectionMatrix", camera);
			glUseProgram(dottedShader);
			shader_set_uniform_mat4(dottedShader, "viewProjectionMatrix", camera);
			shader_set_uniform_float(dottedShader, "time", total_time);
			glUseProgram(stringShader);
			shader_set_uniform_mat4(stringShader, "viewProjectionMatrix", camera);
		}
		//draw full sprites.
		{
			//send it on over to gpu!
			glUseProgram(fullSpriteShader);
			glBindVertexArray(fullsprite_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, fullspriteAtlasBuffer);
			std::cout << "buffer_attempt" << std::endl;
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * fullspriteDraw.num_sprites_drawn, fullspriteDraw.atlas_cpu);

			glBindBuffer(GL_ARRAY_BUFFER, fullspriteMatrixBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * fullspriteDraw.num_sprites_drawn, fullspriteDraw.final_cpu);

			glBindTexture(GL_TEXTURE_2D, floorAtlas);
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, fullspriteDraw.num_sprites_drawn);
		}

		//draw layers
		for (int i = 0; i < GAME_NUM_LAYERS; i++)
		{
			glBindVertexArray(layer_draw[i].VAO);
			glUseProgram(spriteShader);
			glBindBuffer(GL_ARRAY_BUFFER, layer_draw[i].atlas_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * layer_draw[i].total_drawn, layer_draw[i].atlas_cpu);
			glBindBuffer(GL_ARRAY_BUFFER, layer_draw[i].positions_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * layer_draw[i].total_drawn, layer_draw[i].positions_cpu);
			glBindBuffer(GL_ARRAY_BUFFER, layer_draw[i].movement_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * layer_draw[i].total_drawn, layer_draw[i].movement_cpu);
			glBindBuffer(GL_ARRAY_BUFFER, layer_draw[i].color_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * layer_draw[i].total_drawn, layer_draw[i].color_cpu);
			glBindTexture(GL_TEXTURE_2D, layer_draw[i].texture);
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, layer_draw[i].total_drawn);
		}
		//draw floor.
		{
			//send that data to the gpu!
			glBindVertexArray(floor_VAO);
			glUseProgram(spriteShader);
			glBindBuffer(GL_ARRAY_BUFFER, floorAtlasBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * floor_total_drawn, floor_atlas_cpu);
			glBindBuffer(GL_ARRAY_BUFFER, floorPositionsBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * floor_total_drawn, floor_positions_cpu);

			//draw that data!
			glBindTexture(GL_TEXTURE_2D, floorAtlas);
			glBindVertexArray(floor_VAO);
			//glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, floor_total_drawn);
		}
		//draw pieces.
		{
			//send it on over to gpu!
			glUseProgram(spriteShader);
			glBindVertexArray(piece_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, pieceAtlasBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * piece_total_drawn, piece_atlas_cpu);

			glBindBuffer(GL_ARRAY_BUFFER, piecePositionsBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * piece_total_drawn, piece_positions_cpu);
			//draw it!
			glBindTexture(GL_TEXTURE_2D, pieceAtlas);
			glBindVertexArray(piece_VAO);
			//glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, piece_total_drawn);

		}
		//draw dotted lines
		{
			//send it to the gpu!
			glUseProgram(dottedShader);
			glBindVertexArray(dotted_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, dotted_positions_buffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * dotted_total_drawn, dotted_positions_cpu);
			glBindBuffer(GL_ARRAY_BUFFER, dotted_scale_buffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * dotted_total_drawn, dotted_scale_cpu);

			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, dotted_total_drawn);
		}
		//draw text
		{
			glUseProgram(stringShader);

			glBindVertexArray(string_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, string_atlas_buffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * *text_draw_info.current_number_drawn, text_draw_info.string_atlas_cpu);
			glBindBuffer(GL_ARRAY_BUFFER, string_matrix_buffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * *text_draw_info.current_number_drawn, text_draw_info.string_matrix_cpu);

			glBindTexture(GL_TEXTURE_2D, string_texture);
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, *text_draw_info.current_number_drawn);

		}

#pragma endregion
#pragma region Finish and reset for next frame.
		SDL_GL_SwapWindow(window);
		memory_clear(frame_memory);


		//each frame we are sending the entirety of all the floor and piece things to draw to the gpu, so we reset the list we have. goood.
		for (int z = 0; z < GAME_NUM_LAYERS; z++)
			layer_draw[z].total_drawn = 0;
		floor_total_drawn = 0;
		piece_total_drawn = 0;
		fullspriteDraw.num_sprites_drawn = 0;
		dotted_total_drawn = 0;
		*text_draw_info.current_number_drawn = 0;
		memory_clear(frame_memory);
#pragma endregion 
		//return running;
		keep_running_infinite_loop = running;
}
int main(int argc, char *argv[])
{
#pragma region SDL_Setup

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_WINDOW_RESIZABLE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
	//int numBuffers;
	//int numSamples;
	//SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &numBuffers);
	//SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &numSamples);
	//std::cout << "Num buffers" << numBuffers << "Num Samples" << numSamples << std::endl;


	SDL_GLContext mainContext;
	window = SDL_CreateWindow("Castle Elsewhere", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, SCREEN_STARTING_WIDTH, SCREEN_STARTING_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	mainContext = SDL_GL_CreateContext(window);
	if (mainContext == NULL)
	{
		printf("we have failed to initialise the gl context");
	}
	auto rdr = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
#ifndef EMSCRIPTEN
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		return -1;
	}
#endif 

#pragma endregion
#pragma region Memory_AND_TimeMachine_AND_Clock_Setup
	printf("Hello world abc\n");
	frame_memory = memory_create(1000000);
	permanent_memory = memory_create(10000000);
	menu_memory = memory_create(1000000);
	editor_memory = memory_create(10000000);
	play_memory = memory_create(3000000);
	world_memory = memory_create(10000000);
	level_memory = memory_create(10000000);
	animation_memory = memory_create(1000000);
	text_memory = memory_create(100000);
	//Clock* clock = clock_create(permanent_memory);

#pragma endregion
#pragma region Nested GPU Setup
	
	#pragma region Basic Setup
		float vertices[] = {
		 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		};
		float positions[] = {
			0.0f,0.0f,0.0f,
			1.0f,1.0f,0.0f,
			2.0f,2.0f,0.0f,
			3.0f,3.0f,0.0f,
			4.0f,4.0f,0.0f,
			5.0f,5.0f,0.0f
		};
		unsigned int atlas[] = {
			0,
			1,
			2,
			3,
			4,
			5
		};
		unsigned int indices[] = {
			0,1,3,
			1,2,3
		};

		printf("Helllooo no?\n/n)");
		glEnable(GL_DEPTH_TEST);
		printf("HELL YEAH!\n/n");
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND); 
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		spriteShader = shader_compile_program("movesprite.vs", "movesprite.f");
		fullSpriteShader = shader_compile_program("fullsprite.vs", "sprite.f");
		dottedShader = shader_compile_program("dottedlines.vs", "dottedlines.f");
		textShader = shader_compile_program("text.vs", "sprite.f");
		stringShader = shader_compile_program("string.vs", "string.f");
		floorAtlas = resource_load_image_from_file_onto_gpu("FinalFloor.png");
		pieceAtlas = resource_load_image_from_file_onto_gpu("FinalPiece.png");
		glUseProgram(spriteShader);

		unsigned int vertices_VBO;	
		unsigned int vertices_EBO;
		//build useful static vertex buffer objects and index buffer objects.
		{
			glGenBuffers(1, &vertices_VBO);
			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glGenBuffers(1, &vertices_EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		}
	#pragma endregion
	#pragma region Layer GPU setup
			layer_draw = (LayerDrawGPUData*) memory_alloc(permanent_memory, sizeof(LayerDrawGPUData) * GAME_NUM_LAYERS);

			layer_draw[0].atlas_mapper = resource_load_texcoords_floor(permanent_memory, frame_memory);
			layer_draw[1].atlas_mapper = resource_load_texcoords_pieces(permanent_memory, frame_memory);
			layer_draw[0].texture = floorAtlas;
			layer_draw[1].texture = pieceAtlas;
			for (int i = 0; i < GAME_NUM_LAYERS; i++)
			{
				layer_draw[i].total_drawn = 0;
				layer_draw[i].positions_cpu = (glm::vec3*) memory_alloc(permanent_memory, MAX_NUM_FLOOR_SPRITES * sizeof(glm::vec3));
				layer_draw[i].atlas_cpu = (glm::vec4*) memory_alloc(permanent_memory, MAX_NUM_FLOOR_SPRITES * sizeof(glm::vec4));
				layer_draw[i].movement_cpu = (glm::vec2*) memory_alloc(permanent_memory, MAX_NUM_FLOOR_SPRITES * sizeof(glm::vec2));
				layer_draw[i].color_cpu = (glm::vec4*) memory_alloc(permanent_memory, MAX_NUM_FLOOR_SPRITES * sizeof(glm::vec4));
				glUseProgram(spriteShader);
				std::cout << "Layer draw setup begins." << std::endl;
				glGenVertexArrays(1, &layer_draw[i].VAO);
				std::cout << glGetError() << std::endl;
				glBindVertexArray(layer_draw[i].VAO);

				glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);

				GLint position = 0;
				glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(position);

				GLint texcoord = 1;
				glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(texcoord);

				//TODO:

				//build positions.
				glGenBuffers(1, &layer_draw[i].positions_VBO);
				std::cout << glGetError() << std::endl;
				glBindBuffer(GL_ARRAY_BUFFER, layer_draw[i].positions_VBO);
				std::cout << glGetError() << std::endl;
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * MAX_NUM_FLOOR_SPRITES, NULL, GL_DYNAMIC_DRAW);
				std::cout << glGetError() << std::endl;

				GLint positionOffset = 2;
				
				glVertexAttribPointer(positionOffset, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(positionOffset);
				glVertexAttribDivisor(positionOffset, 1);

				//build atlas.
				glGenBuffers(1, &layer_draw[i].atlas_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, layer_draw[i].atlas_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_NUM_FLOOR_SPRITES, NULL, GL_DYNAMIC_DRAW);

				GLint atlasOffset = 3;
				glVertexAttribPointer(atlasOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0));
				glEnableVertexAttribArray(atlasOffset);
				glVertexAttribDivisor(atlasOffset, 1);

				//build movement.
				{
					GLint movementOffset = 4;
					glGenBuffers(1, &layer_draw[i].movement_VBO);
					glBindBuffer(GL_ARRAY_BUFFER, layer_draw[i].movement_VBO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* MAX_NUM_FLOOR_SPRITES, NULL, GL_DYNAMIC_DRAW);
					glVertexAttribPointer(movementOffset, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)(0));
					glEnableVertexAttribArray(movementOffset);
					glVertexAttribDivisor(movementOffset, 1);
				}

				
				//build color.
				{
					GLint colorOffset = 5;
					glGenBuffers(1, &layer_draw[i].color_VBO);
					glBindBuffer(GL_ARRAY_BUFFER, layer_draw[i].color_VBO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_NUM_FLOOR_SPRITES, NULL, GL_DYNAMIC_DRAW);
					glVertexAttribPointer(colorOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0));
					glEnableVertexAttribArray(colorOffset);
					glVertexAttribDivisor(colorOffset, 1);
				}


			}
	#pragma endregion
	#pragma region Floor GPU setup

		floor_positions_cpu = (glm::vec3*) memory_alloc(permanent_memory, MAX_NUM_FLOOR_SPRITES * sizeof(glm::vec3));
		floor_atlas_cpu = (glm::vec4*)memory_alloc(permanent_memory, MAX_NUM_FLOOR_SPRITES * sizeof(glm::vec4));
		floor_atlas_mapper = resource_load_texcoords_floor(permanent_memory, frame_memory);
		floor_total_drawn = 0;
		//build floor VAO
		{

			glGenVertexArrays(1, &floor_VAO);
			glBindVertexArray(floor_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);

			GLint position = 0;
			glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(position);

		
			GLint texCoord = 1;
			glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(texCoord);
		

			glGenBuffers(1, &floorPositionsBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, floorPositionsBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * MAX_NUM_FLOOR_SPRITES, NULL, GL_DYNAMIC_DRAW);
		
		
			GLint positionOffset = glGetAttribLocation(spriteShader, "positionOffset");

			glVertexAttribPointer(positionOffset, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(positionOffset);
			glVertexAttribDivisor(positionOffset, 1);
		

			glGenBuffers(1, &floorAtlasBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, floorAtlasBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_NUM_FLOOR_SPRITES, NULL, GL_DYNAMIC_DRAW);

		
			GLint atlasOffset = glGetAttribLocation(spriteShader, "atlasCoord");
			glVertexAttribPointer(atlasOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0));
			glEnableVertexAttribArray(atlasOffset);
			glVertexAttribDivisor(atlasOffset, 1);
		}
	#pragma endregion 
	#pragma region Piece GPU setup
		piece_VAO;
		piecePositionsBuffer;
		pieceAtlasBuffer;
		piece_atlas_cpu = (glm::vec4*) memory_alloc(permanent_memory, MAX_NUM_FLOOR_SPRITES * sizeof(glm::vec4));
		piece_positions_cpu = (glm::vec3*) memory_alloc(permanent_memory, MAX_NUM_FLOOR_SPRITES * sizeof(glm::vec3));
		piece_atlas_mapper = resource_load_texcoords_pieces(permanent_memory, frame_memory);
		int piece_total_drawn = 0;
		//build piece VAO
		{
			glGenVertexArrays(1, &piece_VAO);
			glBindVertexArray(piece_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);

			GLint position = 0;
			glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(position);

			GLint texCoord = 1;
			glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(texCoord);

			glGenBuffers(1, &piecePositionsBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, piecePositionsBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * MAX_NUM_FLOOR_SPRITES, NULL, GL_DYNAMIC_DRAW);

			GLint positionOffset = glGetAttribLocation(spriteShader, "positionOffset");
			glVertexAttribPointer(positionOffset, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(positionOffset);
			glVertexAttribDivisor(positionOffset, 1);

			glGenBuffers(1, &pieceAtlasBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, pieceAtlasBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)* MAX_NUM_FLOOR_SPRITES, NULL, GL_DYNAMIC_DRAW);

			GLint atlasOffset = glGetAttribLocation(spriteShader, "atlasCoord");
			glVertexAttribPointer(atlasOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0));
			glEnableVertexAttribArray(atlasOffset);
			glVertexAttribDivisor(atlasOffset, 1);
		}
	#pragma endregion 
	#pragma region fullsprite GPU setup
		fullspriteDraw.num_sprites_drawn = 0;
		fullspriteDraw.atlas_cpu = (glm::vec4*) memory_alloc(permanent_memory, MAX_NUM_FULL_SPRITES * sizeof(glm::vec4));
		fullspriteDraw.atlas_mapper = floor_atlas_mapper;
		fullspriteDraw.final_cpu = (glm::mat4*) memory_alloc(permanent_memory, MAX_NUM_FULL_SPRITES * sizeof(glm::mat4));;
		shader_use(fullSpriteShader);
		//just use floor_atlas_mapper for fullsprite_atlas_mapper
		{
			glGenVertexArrays(1, &fullsprite_VAO);
			std::cout << glGetError() << ":261" << std::endl;
			glBindVertexArray(fullsprite_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);

			int position = glGetAttribLocation(fullSpriteShader, "pos");
			std::cout << glGetError() << " this is our position value" << std::endl;
			glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(position);

			int texCoord = glGetAttribLocation(fullSpriteShader, "inputTexCoord");
			std::cout << glGetError() << " this is our texCoord value" << std::endl;
			glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(texCoord);


			glGenBuffers(1, &fullspriteMatrixBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, fullspriteMatrixBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * MAX_NUM_FULL_SPRITES, NULL, GL_DYNAMIC_DRAW);

			int matrixOffset = 3;	//value hardcoded from fullsprite.vs
			glVertexAttribPointer(matrixOffset, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)0);
			std::cout << glGetError() << std::endl; 
			std::cout << matrixOffset << "This is our matrix offset" << std::endl;
			glVertexAttribPointer(matrixOffset + 1, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(4 * sizeof(float)));

			glVertexAttribPointer(matrixOffset + 2, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(8 * sizeof(float)));
			glVertexAttribPointer(matrixOffset + 3, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(12 * sizeof(float)));
			glEnableVertexAttribArray(matrixOffset);
			glEnableVertexAttribArray(matrixOffset + 1);
			glEnableVertexAttribArray(matrixOffset + 2);
			glEnableVertexAttribArray(matrixOffset + 3);
			glVertexAttribDivisor(matrixOffset, 1);
			glVertexAttribDivisor(matrixOffset + 1, 1);
			glVertexAttribDivisor(matrixOffset + 2, 1);
			glVertexAttribDivisor(matrixOffset + 3, 1);

			glGenBuffers(1, &fullspriteAtlasBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, fullspriteAtlasBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)* MAX_NUM_FULL_SPRITES, NULL, GL_DYNAMIC_DRAW);

			int atlasOffset = glGetAttribLocation(fullSpriteShader, "atlasCoord");
			glVertexAttribPointer(atlasOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0));
			glEnableVertexAttribArray(atlasOffset);
			glVertexAttribDivisor(atlasOffset, 1);
		}
	#pragma endregion 
	#pragma region dotted GPU setup
		dotted_VAO;
		dotted_positions_buffer;
		dotted_scale_buffer;
		dotted_total_drawn = 0;
		dotted_positions_cpu = (glm::vec3*) memory_alloc(permanent_memory, MAX_NUM_DOTTED_SPRITES * sizeof(glm::vec3));
		dotted_scale_cpu = (glm::vec2*) memory_alloc(permanent_memory, MAX_NUM_DOTTED_SPRITES * sizeof(glm::vec2));
		{
			glGenVertexArrays(1, &dotted_VAO);
			glBindVertexArray(dotted_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);

			int position = glGetAttribLocation(dottedShader, "pos");
			glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(position);

			glGenBuffers(1, &dotted_positions_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, dotted_positions_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* MAX_NUM_DOTTED_SPRITES, NULL, GL_DYNAMIC_DRAW);

			int position_offset = glGetAttribLocation(dottedShader, "positionOffset");
			glVertexAttribPointer(position_offset, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(position_offset);
			glVertexAttribDivisor(position_offset, 1);
			std::cout << "pos new stuff" << std::endl;
			glGenBuffers(1, &dotted_scale_buffer);
			std::cout << glGetError() << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, dotted_scale_buffer);
			std::cout << glGetError() << std::endl;
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* MAX_NUM_DOTTED_SPRITES, NULL, GL_DYNAMIC_DRAW);
			std::cout << glGetError() << std::endl;

			int scale = glGetAttribLocation(dottedShader, "scale");
			std::cout << glGetError() << std::endl;
			glVertexAttribPointer(scale, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			std::cout << glGetError() << std::endl;
			glEnableVertexAttribArray(scale);
			std::cout << glGetError() << std::endl;
			glVertexAttribDivisor(scale, 1);
			std::cout << glGetError() << std::endl;
		}

	#pragma endregion
	#pragma region string GPU setup
		shader_use(stringShader);
		text_draw_info.current_number_drawn = (int*) memory_alloc(permanent_memory,sizeof(int));
		text_draw_info.string_atlas_cpu = (glm::vec4*) memory_alloc(permanent_memory, MAX_NUM_CHARACTERS * sizeof(glm::vec4));
		text_draw_info.string_matrix_cpu = (glm::mat4*) memory_alloc(permanent_memory, MAX_NUM_CHARACTERS * sizeof(glm::mat4));
		{
			glGenVertexArrays(1, &string_VAO);
			std::cout << glGetError() << ":261" << std::endl;
			glBindVertexArray(string_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);

			int position = 0;
			glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(position);

			int texCoord = 1;
			glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(texCoord);

			glGenBuffers(1, &string_matrix_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, string_matrix_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4)* MAX_NUM_CHARACTERS, NULL, GL_DYNAMIC_DRAW);

			int matrixOffset = 3;	//value hardcoded from text.vs
			glVertexAttribPointer(matrixOffset, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)0);
			glVertexAttribPointer(matrixOffset + 1, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(4 * sizeof(float)));

			glVertexAttribPointer(matrixOffset + 2, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(8 * sizeof(float)));
			glVertexAttribPointer(matrixOffset + 3, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(12 * sizeof(float)));
			glEnableVertexAttribArray(matrixOffset);
			glEnableVertexAttribArray(matrixOffset + 1);
			glEnableVertexAttribArray(matrixOffset + 2);
			glEnableVertexAttribArray(matrixOffset + 3);
			glVertexAttribDivisor(matrixOffset, 1);
			glVertexAttribDivisor(matrixOffset + 1, 1);
			glVertexAttribDivisor(matrixOffset + 2, 1);
			glVertexAttribDivisor(matrixOffset + 3, 1);

			glGenBuffers(1, &string_atlas_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, string_atlas_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)* MAX_NUM_CHARACTERS, NULL, GL_DYNAMIC_DRAW);

			int atlasOffset = 2;
			glVertexAttribPointer(atlasOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0));
			glEnableVertexAttribArray(atlasOffset);
			glVertexAttribDivisor(atlasOffset, 1);
		}
	#pragma endregion 
	#pragma region Camera Setup And UtilStructs
		//setup all the information we just initialized into a handy struct.
		{
			gamespriteDrawInfo.floor_atlas_cpu = floor_atlas_cpu;
			gamespriteDrawInfo.floor_atlas_mapper = floor_atlas_mapper;
			gamespriteDrawInfo.floor_positions_cpu = floor_positions_cpu;
			gamespriteDrawInfo.floor_total_drawn = &floor_total_drawn;
			gamespriteDrawInfo.piece_atlas_cpu = piece_atlas_cpu;
			gamespriteDrawInfo.piece_atlas_mapper = piece_atlas_mapper;
			gamespriteDrawInfo.piece_positions_cpu = piece_positions_cpu;
			gamespriteDrawInfo.piece_total_drawn = &piece_total_drawn;
		}


		//calculate the starting camera position.
		{
			float camera_left_x = 0;
			float camera_bottom_y = 0;
			float camera_right_x;
			float camera_top_y;
			//calculate camera's top and right positions.
			{
				int camera_width = screen_width;
				int camera_height = screen_height;
				float ratio = (float)camera_width / (float)camera_height;
				camera_right_x = GAME_HEIGHT_START * ratio;
				camera_top_y = GAME_HEIGHT_START;
			}

			camera_game.left = camera_left_x;
			camera_game.right = camera_right_x;
			camera_game.up = camera_top_y;
			camera_game.down = camera_bottom_y;
			camera_game.closePoint = -20;
			camera_game.farPoint = 20.0f;

			camera_viewport.left = 0;
			camera_viewport.right = screen_width;
			camera_viewport.down = 0;
			camera_viewport.up = screen_height;
		}

		camera = camera_make_matrix(camera_game);
		//shader_set_uniform_mat4(spriteShader, "viewProjectionMatrix", camera);
	#pragma endregion
	#pragma region TTF font setup

		FT_Library ft;
		if(FT_Init_FreeType(&ft))
			std::cout << "ERROR - freetype library load failed. alas." << std::endl;
		else
			std::cout << "successfully loaded freetype library" << std::endl;
		FT_Face face;
		if (FT_New_Face(ft, "assets/arial.ttf", 0, &face))
			std::cout << "ERROR - freetype failed to load font." << std::endl;
		else
			std::cout << "successfully loaded freetype font." << std::endl;
		FT_Set_Pixel_Sizes(face, 0, FONT_CHARACTER_HEIGHT);
		//setup the gpu work.
		GLuint text_VAO;
		{
			glGenVertexArrays(1, &text_VAO);
			glBindVertexArray(text_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);

			GLint position = glGetAttribLocation(textShader, "pos");
			glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(position);

			GLint texCoord = glGetAttribLocation(textShader, "texCoords");
			glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(texCoord);
		}
		//Setup the text.
		string_texture;
		text_draw_info.text_positions = (AABB*) memory_alloc(permanent_memory, sizeof(AABB) * FONT_NUM_CHARACTERS);
		text_draw_info.text_positions_normalized = (AABB*) memory_alloc(permanent_memory, sizeof(AABB) * FONT_NUM_CHARACTERS);
		unsigned int* char_texture = (unsigned int*)memory_alloc(permanent_memory, sizeof(unsigned int) * FONT_NUM_CHARACTERS);
		text_draw_info.true_font_reference = (TTF_Character*)memory_alloc(permanent_memory, sizeof(TTF_Character) * FONT_NUM_CHARACTERS);
		{
			int current_width_written = 0;
			int current_height_written = 0;
			unsigned int text_FBO;
			glGenFramebuffers(1, &text_FBO);
			glBindFramebuffer(GL_FRAMEBUFFER, text_FBO);

			glGenTextures(1,&string_texture);
			glBindTexture(GL_TEXTURE_2D, string_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, string_texture, 0);

			glGenTextures(FONT_NUM_CHARACTERS, char_texture);
			glBindVertexArray(text_VAO);
			glUseProgram(textShader);

			FT_Set_Pixel_Sizes(face, 0, FONT_CHARACTER_HEIGHT);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glViewport(0, 0, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR - Framebuffer is not complete!" << std::endl;
			else
				std::cout << "framebuffer complete!" << std::endl;

			for (unsigned char c = 0; c < FONT_NUM_CHARACTERS; c++)
			{
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					std::cout << "ERROR - uh oh, failed to load character glyph" << std::endl;
					continue;
				}

				//generate a texture for this particular character.
				int face_width = face->glyph->bitmap.width;
				int face_height = face->glyph->bitmap.rows;
				{
					unsigned int textureID = char_texture[c];
					glBindTexture(GL_TEXTURE_2D, char_texture[c]);
					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_R8, 
						face->glyph->bitmap.width, 
						face->glyph->bitmap.rows, 
						0, 
						GL_RED,
						GL_UNSIGNED_BYTE, 
						face->glyph->bitmap.buffer);
					std::cout << glGetError() << std::endl;
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
				//write the generated texture to the atlas.
				{
					//check if their is room to write. If yes, success. If nay, don't belay
					int write_width = face_width + 2;
					bool room_to_write = write_width + current_width_written < FONT_ATLAS_WIDTH;
					if (!room_to_write)
					{
						current_width_written = 0;
						current_height_written += FONT_CHARACTER_HEIGHT + 2;
					}

					//2: write to our atlas info about where the character will be written too.
					AABB temp = math_AABB_create((float) (current_width_written + 1), (float) (current_height_written + 1), (float) face_width, (float) face_height);
					text_draw_info.text_positions[c] = temp;
					text_draw_info.text_positions_normalized[c] = text_draw_info.text_positions[c];
					text_draw_info.text_positions_normalized[c].x = text_draw_info.text_positions[c].x / FONT_ATLAS_WIDTH;
					text_draw_info.text_positions_normalized[c].y = text_draw_info.text_positions[c].y / FONT_ATLAS_HEIGHT;
					text_draw_info.text_positions_normalized[c].w = text_draw_info.text_positions[c].w / FONT_ATLAS_WIDTH;
					text_draw_info.text_positions_normalized[c].h = text_draw_info.text_positions[c].h / FONT_ATLAS_HEIGHT;
					//ahhhhh, so. SO!
					//3: actually write the character.
					AABB temp2 = text_draw_info.text_positions_normalized[c];
					glm::vec4 temp3 = *(glm::vec4*) & temp2;
					shader_set_uniform_vec4(textShader, "offset", temp3);
					current_width_written += write_width;
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

					text_draw_info.true_font_reference[c].Size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
					text_draw_info.true_font_reference[c].Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
					text_draw_info.true_font_reference[c].advance = (face->glyph->advance.x);
				}
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			//cleanup.
			glDeleteFramebuffers(1,&text_FBO);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, screen_width, screen_height);
		}
	#pragma endregion

	#pragma endregion
#pragma region MAIN_LOOP_INIT
		//MAIN LOOP
		//	scene = ST_EDITOR;

		scene = ST_MENU;
		menu_scene_state = setup_main_menu(menu_memory, menu_action_new_game, menu_action_continue_game, menu_action_level_editor);
		//setup ui
		ui_state = click_ui_init(permanent_memory);
		//time machine setup
		//editor_scene_state = editorscene_setup(editor_memory, camera_viewport);
	#pragma endregion

#ifdef EMSCRIPTEN
		std::string to_load = resource_load_puzzle_file("world3");
		load_editor_level_stateful(to_load);
		setup_world_screen_stateful();
		emscripten_set_main_loop(mainloopfunction, 0, 0);
#else
		while (true)
		{
			mainloopfunction();
			if (!keep_running_infinite_loop)
				break;
		}
		//cleanup.
		{
			printf("%s\n", glGetString(GL_VERSION));
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
#endif 
		return 0;
}

void take_player_action(Animations** maybe_animation, GamestateTimeMachine* maybe_time_machine, IntPair draw_position, EditorUIState* ui_state, Direction action, Memory* level_memory, Memory* frame_memory, Memory* animation_memory)
{
	GameActionJournal* journal = gamestate_timemachine_take_action(maybe_time_machine, action, level_memory, frame_memory);
	GameStateAnimation* animation = journal->maybe_animation;
	ui_state->time_till_player_can_move = WAIT_BETWEEN_PLAYER_MOVE_REPEAT;
	ui_state->time_since_last_player_action = 0;
	Animations* a = animation_build_from_world(journal, maybe_time_machine, draw_position, animation_memory);
	*maybe_animation = a;
}
/*
float draw_text_to_screen(glm::vec3 start_position, 
	const char* c_string, 
	glm::mat4* matrix, 
	glm::vec4* atlas_output, 
	TTF_Character* true_font_reference,
	AABB* atlas_reference, 
	AABB* normalized_atlas_reference, 
	int* current_draw,
	float num_pixels_per_gameunit)
{
	glm::vec3 next_position = start_position;
	int draw = *current_draw;
	for(;c_string[0]; c_string++)
	{
		char c = c_string[0];

		//handle the position of the character.
		float horizontal_space = atlas_reference[c].w / FONT_CHARACTER_HEIGHT;

		float x_start = (next_position.x + true_font_reference[c].Bearing.x) / float(FONT_CHARACTER_HEIGHT);
		float y_start = (next_position.y - (true_font_reference[c].Size.y - true_font_reference[c].Bearing.y)) / float(FONT_CHARACTER_HEIGHT);
		float w = (true_font_reference[c].Size.x) / float(FONT_CHARACTER_HEIGHT);
		float h = true_font_reference[c].Size.y / float(FONT_CHARACTER_HEIGHT);

		if (c != ' ')
		{
			glm::vec3 start_position = glm::vec3(next_position.x + x_start, next_position.y + y_start, next_position.z);
			matrix[draw] = glm::mat4(1.0f);
			matrix[draw] = glm::translate(matrix[draw], start_position);
			matrix[draw] = glm::scale(matrix[draw], glm::vec3(w, h, 1));

			//handle what character should be drawn.
			AABB box = normalized_atlas_reference[c];
			glm::vec4 to_atlas_output = glm::vec4(box.x, box.y, box.x + box.w, box.y + box.h);
			atlas_output[draw] = to_atlas_output;

			draw++;
		}

		//we always increment the draw foward, even when we don't draw stuff like an empty space.
		float x_pixels_in_gamespace = (true_font_reference[c].advance >> 6) / float(FONT_CHARACTER_HEIGHT);
		next_position.x += x_pixels_in_gamespace;

	}
	*current_draw = draw;
	return next_position.x - start_position.x;
}
*/

float draw_text_to_screen(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextDrawInfo* info)
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
		float y_start = (next_position.y - (info->true_font_reference[c].Size.y - info->true_font_reference[c].Bearing.y) * scale.y) / float(FONT_CHARACTER_HEIGHT);
		float w = (info->true_font_reference[c].Size.x) / float(FONT_CHARACTER_HEIGHT);
		float h = info->true_font_reference[c].Size.y / float(FONT_CHARACTER_HEIGHT);

		if (c != ' ')
		{
			glm::vec3 start_position = glm::vec3(next_position.x + x_start, next_position.y + y_start, next_position.z);
			info->string_matrix_cpu[draw] = glm::mat4(1.0f);
			info->string_matrix_cpu[draw] = glm::translate(info->string_matrix_cpu[draw], start_position);
			info->string_matrix_cpu[draw] = glm::scale(info->string_matrix_cpu[draw], glm::vec3(w * scale.x, h * scale.y, 1));

			//handle what character should be drawn.
			AABB box = info->text_positions_normalized[c];
			glm::vec4 to_atlas_output = glm::vec4(box.x, box.y, box.x + box.w, box.y + box.h);
			info->string_atlas_cpu[draw] = to_atlas_output;

			draw++;
		}

		//we always increment the draw foward, even when we don't draw stuff like an empty space.
		float x_pixels_in_gamespace = (info->true_font_reference[c].advance >> 6) / float(FONT_CHARACTER_HEIGHT);
		next_position.x += (x_pixels_in_gamespace * scale.x);

	}
	*info->current_number_drawn = draw;
	return next_position.x - start_position.x;
}
AABB calculate_outline_from_create_info(Memory* frame_memory, TimeMachineEditor* timeMachine, EditorUIState ui_state)
{
	IntPair current_position = math_intpair_create((int) floor(ui_state.mouseGamePos.x), (int) floor(ui_state.mouseGamePos.y));
	IntPair bottom_left = math_intpair_create(mini(current_position.x, ui_state.un.create.gameworld_start_pos.x),
		mini(current_position.y, ui_state.un.create.gameworld_start_pos.y));
	IntPair top_right = math_intpair_create(maxi(current_position.x, ui_state.un.create.gameworld_start_pos.x),
		maxi(current_position.y, ui_state.un.create.gameworld_start_pos.y));
	int w = top_right.x - bottom_left.x + 1;
	int h = top_right.y - bottom_left.y + 1;
	AABB next_size = math_AABB_create((float) bottom_left.x, (float) bottom_left.y, (float) w, (float) h);
	return next_size;
}
AABB calculate_outline_from_move_info(Memory* frame_memory, TimeMachineEditor* timeMachine, EditorUIState ui_state)
{
	//calculate the distance between that space and the starting space.
	glm::vec2 distance = ui_state.mouseGamePos - ui_state.un.move.move_start_position;
	//transfer that into an int offset to apply to the moved gamestate.
	IntPair offset = math_intpair_create((int)distance.x, (int)distance.y);
	int gamestate_index = ui_state.un.move.moving_gamestate_index;
	GameState* current_gamestate = timeMachine->gamestates[gamestate_index];
	IntPair startPosition = timeMachine->gamestates_positions[gamestate_index];
	AABB outline = math_AABB_create((float) (startPosition.x + offset.x), (float) (startPosition.y + offset.y), (float) current_gamestate->w, (float) current_gamestate->h);
	return outline;
}
AABB calculate_outline_position_from_drag_info(Memory* frame_memory,
	TimeMachineEditor* timeMachine,
	EditorUIState ui_state, 
	glm::vec2 dragging_start_position_in_gamespace)
{
	//determine (using ints to round down) how far away from the starting position we are.
	glm::vec2 offset = ui_state.mouseGamePos - dragging_start_position_in_gamespace;
	//get the width and height of the targetted gamestate.
	IntPair targetted_gamestate_position = timeMachine->gamestates_positions[ui_state.un.resize.dragging_gamestate_index];
	GameState* targetted_gamestate = timeMachine->gamestates[ui_state.un.resize.dragging_gamestate_index];
	int targetted_gamestate_width = targetted_gamestate->w;
	int targetted_gamestate_height = targetted_gamestate->h;

	AABB old = math_AABB_create_int(targetted_gamestate_position.x,
		targetted_gamestate_position.y,
		targetted_gamestate_width,
		targetted_gamestate_height);
	AABB next = old;

	float offset_x = sign(offset.x) * 0.5f;
	float offset_y = sign(offset.y) * 0.5f;
	if (ui_state.un.resize.dragging_left)
	{
		int left_movement = (int)(ui_state.mouseGamePos.x - ((float)next.x) + offset_x);
		if (left_movement != 0)
		{
			next.x += left_movement;
			next.w -= left_movement;
		}
	}
	if (ui_state.un.resize.dragging_right)
	{
		int right_movement = (int)(ui_state.mouseGamePos.x - ((float)next.x + next.w) + offset_x);
		if (right_movement != 0)
			next.w += right_movement;
	}
	if (ui_state.un.resize.dragging_down)
	{
		int down_movement = (int)(ui_state.mouseGamePos.y - ((float)next.y) + offset_y);
		if (down_movement != 0)
		{
			next.y += down_movement;
			next.h -= down_movement;
		}
	}
	if (ui_state.un.resize.dragging_up)
	{
		int up_movement = (int)(ui_state.mouseGamePos.y - ((float)next.y + next.h) + offset_y);
		if (up_movement != 0)
			next.h += up_movement;
	}

	bool changeOccured = next.x != targetted_gamestate_position.x
		|| next.y != targetted_gamestate_position.y
		|| next.w != targetted_gamestate->w
		|| next.h != targetted_gamestate->h;

	//handle the edge case where we resize too far and the gameobject would be destroyed.
	if (next.w <= 0)
	{
		int go_back = (int) next.w - 1;
		next.w = 1;
		if (ui_state.un.resize.dragging_left)
		{
			next.x += go_back;
		}
	}

	if (next.h <= 0)
	{
		int go_back = (int) next.h - 1;
		next.h = 1;
		if (ui_state.un.resize.dragging_down)
		{
			next.y += go_back;
		}
	}
	return next;
}
void draw_black_box_over_screen(GameSpaceCamera screen, GamefullspriteDrawInfo* info)
{
	info->final_cpu[info->num_sprites_drawn] = glm::mat4(1.0f);
	info->final_cpu[info->num_sprites_drawn] = glm::translate(info->final_cpu[info->num_sprites_drawn], glm::vec3(screen.left, screen.down, 10));
	float height = screen.up - screen.down;
	float width = screen.right - screen.left;
	info->final_cpu[info->num_sprites_drawn] = glm::scale(info->final_cpu[info->num_sprites_drawn], glm::vec3(width,height, 1));
	info->atlas_cpu[info->num_sprites_drawn] = info->atlas_mapper[F_ZBLACK];
	info->num_sprites_drawn++;
}
void draw_outline_to_gamespace(AABB outline, GamefullspriteDrawInfo* info)
{
	info->final_cpu[info->num_sprites_drawn] = glm::mat4(1.0f);
	info->final_cpu[info->num_sprites_drawn] = glm::translate(info->final_cpu[info->num_sprites_drawn], glm::vec3(outline.x, outline.y, 5));
	info->final_cpu[info->num_sprites_drawn] = glm::scale(info->final_cpu[info->num_sprites_drawn], glm::vec3(outline.w, outline.h, 1));
	info->atlas_cpu[info->num_sprites_drawn] = info->atlas_mapper[F_ZBLACK];
	info->num_sprites_drawn++;
}
void draw_gamestates_outlines_to_gamespace(GameState** gamestates,IntPair* offsets,int length_function_input,GamefullspriteDrawInfo* info,int skip_index)
{
	for (int z = 0; z < skip_index; z++)
	{
		int w = gamestates[z]->w;
		int h = gamestates[z]->h;
		IntPair startPos = offsets[z];
		glm::mat4 final = glm::mat4(1.0f);
		final = glm::translate(final, glm::vec3(startPos.x - OUTLINE_DRAW_SIZE, startPos.y - OUTLINE_DRAW_SIZE, 0));
		final = glm::scale(final, glm::vec3(w + OUTLINE_DRAW_SIZE * 2, h + OUTLINE_DRAW_SIZE * 2, 1));
		info->final_cpu[info->num_sprites_drawn] = final;
		info->atlas_cpu[info->num_sprites_drawn] = info->atlas_mapper[F_ZBLACK];
		info->num_sprites_drawn++;
	}
	for (int z = skip_index+1; z < length_function_input; z++)
	{
		int w = gamestates[z]->w;
		int h = gamestates[z]->h;
		IntPair startPos = offsets[z];
		glm::mat4 final = glm::mat4(1.0f);
		final = glm::translate(final, glm::vec3(startPos.x - OUTLINE_DRAW_SIZE, startPos.y - OUTLINE_DRAW_SIZE, 0));
		final = glm::scale(final, glm::vec3(w + OUTLINE_DRAW_SIZE * 2, h + OUTLINE_DRAW_SIZE * 2, 1));
		info->final_cpu[info->num_sprites_drawn] = final;
		info->atlas_cpu[info->num_sprites_drawn] = info->atlas_mapper[F_ZBLACK];
		info->num_sprites_drawn++;
	}
}

void draw_stationary_piece_curses_to_gamespace(MovementAnimation* animation, 
	DrawCurseAnimation* curse_animation,
	GameState** gamestates, 
	IntPair* offsets,
	int number_of_gamestates,
	LayerDrawGPUData* info, 
	int layer_index,
	float time_since_last_action)
{
	int i = layer_index;
	for (int z = 0; z < number_of_gamestates; z++)
	{
		GameState* gamestate = gamestates[z];
		IntPair offset = offsets[z];
		int num_elements_in_gamestate = gamestate->w * gamestate->h;
		for (int k = 0; k < num_elements_in_gamestate; k++)
		{
			int ele = gamestate->layers[i][k];
			CursedDirection curse_status = get_entities_cursed_direction(ele);
			if (curse_status != CursedDirection::NOTCURSED && 
				(!animation || (animation->start_offset[k].x == animation->end_offset[k].x && animation->start_offset[k].y == animation->end_offset[k].y)))
			{
				int curse_to_draw = resource_cursed_direction_to_piece_sprite(curse_status);
				info[i].atlas_cpu[info[i].total_drawn] = info[i].atlas_mapper[curse_to_draw];
				IntPair p = t2D(k, gamestate->w, gamestate->h);
				info[i].positions_cpu[info[i].total_drawn] = glm::vec3(offset.x + p.x, offset.y + p.y, Z_POSITION_STARTING_LAYER + i + 0.2f);
				info[i].movement_cpu[info[i].total_drawn] = glm::vec2(0, 0);
				if (curse_animation->flash[k])
				{
					float l = time_since_last_action / WAIT_BETWEEN_PLAYER_MOVE_REPEAT;
					l = minf(1.0f, l);
					float ln = l * NUM_CURSE_FLASHES * 2;
					float flash_value = (float) abs(fmod(ln, 2) - 1.0);
					glm::vec4 color = glm::mix(glm::vec4(0, 0, 0, 0), glm::vec4(1, 1, 1, 1), flash_value);
					info[i].color_cpu[info[i].total_drawn] = color;
				}
				else
				{
					info[i].color_cpu[info[i].total_drawn] = glm::vec4(1, 1, 1, 1);
				}
				info[i].total_drawn++;
			}

		}
	}
}

void draw_piece_curses_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index, DrawCurseAnimation* curse, float time_since_last_action)
{
	int i = layer_index;
	for (int z = 0; z < number_of_gamestates; z++)
	{
		GameState* gamestate = gamestates[z];
		IntPair offset = offsets[z];
		int num_elements_in_gamestate = gamestate->w * gamestate->h;
		for (int k = 0; k < num_elements_in_gamestate; k++)
		{
			int ele = gamestate->layers[i][k];
			CursedDirection curse_status = get_entities_cursed_direction(ele);
			if (curse_status != CursedDirection::NOTCURSED)
			{
				int curse_to_draw = resource_cursed_direction_to_piece_sprite(curse_status);
				info[i].atlas_cpu[info[i].total_drawn] = info[i].atlas_mapper[curse_to_draw];
				IntPair p = t2D(k, gamestate->w, gamestate->h);
				info[i].positions_cpu[info[i].total_drawn] = glm::vec3(offset.x + p.x, offset.y + p.y, Z_POSITION_STARTING_LAYER + i + 0.2f);
				info[i].movement_cpu[info[i].total_drawn] = glm::vec2(0, 0);
				if (curse && time_since_last_action < WAIT_BETWEEN_PLAYER_MOVE_REPEAT)
				{
					float l = time_since_last_action / WAIT_BETWEEN_PLAYER_MOVE_REPEAT;
					float ln = l * NUM_CURSE_FLASHES * 2;
					float flash_value = (float) abs(fmod(ln, 2) - 1);
					glm::vec4 color = glm::mix(glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 1, 1), flash_value);
					info[i].color_cpu[info[i].total_drawn] = color;
				}
				else
					info[i].color_cpu[info[i].total_drawn] = glm::vec4(1, 1, 1, 1);
				info[i].total_drawn++;
			}

		}
	}
}
bool draw_animation_to_gamespace(MovementAnimation* animation, LayerDrawGPUData* info_array, int layer_index, float time_since_last_action)
{
	float l = time_since_last_action / WAIT_BETWEEN_PLAYER_MOVE_REPEAT;

	l = maxf(0, l);
	l = minf(1, l);

	LayerDrawGPUData* info = &info_array[layer_index];
	int len = animation->num_elements;
	for (int i = 0; i < len; i++)
	{
		int ele = resource_layer_value_to_layer_sprite_value(animation->sprite_value[i],layer_index);
		if (ele == 0)
			continue;
		info->atlas_cpu[info->total_drawn] = info->atlas_mapper[ele];
		info->positions_cpu[info->total_drawn] = animation->start_position[i];
		info->movement_cpu[info->total_drawn] = animation->start_offset[i] * (1.0f - l) + animation->end_offset[i] * l;
		info->color_cpu[info->total_drawn] = glm::vec4(1, 1, 1, 1);
		info->total_drawn++;
	}

	return l >= 1;
}
void draw_curse_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info)
{
	int i = LN_PIECE;
	for (int z = 0; z < number_of_gamestates; z++)
	{
		GameState* gamestate = gamestates[z];
		IntPair offset = offsets[z];
		int num_elements_in_gamestate = gamestate->w * gamestate->h;
		for (int k = 0; k < num_elements_in_gamestate; k++)
		{
			int ele = gamestate->layers[i][k];
			CursedDirection curse_status = get_entities_cursed_direction(ele);
			if (curse_status != CursedDirection::NOTCURSED)
			{
				int curse_to_draw = resource_cursed_direction_to_piece_sprite(curse_status);
				info[i].atlas_cpu[info[i].total_drawn] = info[i].atlas_mapper[curse_to_draw];
				IntPair p = t2D(k, gamestate->w, gamestate->h);
				info[i].positions_cpu[info[i].total_drawn] = glm::vec3(offset.x + p.x, offset.y + p.y, Z_POSITION_STARTING_LAYER + i + 0.2f);
				info[i].movement_cpu[info[i].total_drawn] = glm::vec2(0, 0);
				info[i].color_cpu[info[i].total_drawn] = glm::vec4(1, 1, 1, 1);
				info[i].total_drawn++;
			}

		}
	}
}
void draw_layer_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index)
{
	int i = layer_index;
	{
		for (int z = 0; z < number_of_gamestates; z++)
		{
			GameState* gamestate = gamestates[z];
			IntPair offset = offsets[z];

			int num_elements_in_gamestate = gamestate->w * gamestate->h;
			for (int k = 0; k < num_elements_in_gamestate; k++)
			{
				int ele = gamestate->layers[i][k];
				int ele_image = resource_layer_value_to_layer_sprite_value(ele, i);
				info[i].atlas_cpu[info[i].total_drawn + k] = info[i].atlas_mapper[ele_image];
				info[i].movement_cpu[info[i].total_drawn + k] = glm::vec2(0, 0);
				IntPair p = t2D(k, gamestate->w, gamestate->h);
				info[i].positions_cpu[info[i].total_drawn + k] = glm::vec3(offset.x + p.x, offset.y + p.y, Z_POSITION_STARTING_LAYER + i);
				info[i].color_cpu[info[i].total_drawn + k] = glm::vec4(1, 1, 1, 1);
			}
			info[i].total_drawn += num_elements_in_gamestate;
		}
	}
}
void draw_layers_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info)
{
	for (int i = 0; i < GAME_NUM_LAYERS; i++)
	{
		draw_layer_to_gamespace(gamestates, offsets, number_of_gamestates, info, i);
	}
}

void draw_palette(IntPair palete_screen_start, 
	GameSpaceCamera camera_game, 
	ViewPortCamera camera_viewport, 
	EditorUIState* ui_state, 
	int palete_length, 
	GamestateBrush* palete,
	LayerDrawGPUData* layer_draw)
{
	//parse palette data to gpu form
	{
		//determine what the gamespace position is from the screen position BOTTOM LEFT.
		glm::vec2 palete_true_start = math_screenspace_to_gamespace(palete_screen_start, camera_game, camera_viewport, ui_state->game_height_current);
		for (int i = 0; i < palete_length; i++)
		{
			if (palete[i].applyFloor)
			{
				LayerDrawGPUData* floor = &layer_draw[LN_FLOOR];
				int ele_image = resource_layer_value_to_layer_sprite_value(palete[i].floor, LN_FLOOR);
				floor->atlas_cpu[floor->total_drawn] = floor->atlas_mapper[ele_image];
				floor->positions_cpu[floor->total_drawn] = glm::vec3(palete_true_start.x + i, palete_true_start.y, 4);
				floor->movement_cpu[floor->total_drawn] = glm::vec2(0, 0);
				floor->color_cpu[floor->total_drawn] = glm::vec4(1, 1, 1, 1);
				floor->total_drawn++;
			}
			if (palete[i].applyPiece)
			{
				LayerDrawGPUData* piece = &layer_draw[LN_PIECE];
				int ele_image = resource_layer_value_to_layer_sprite_value(palete[i].piece, LN_PIECE);
				piece->atlas_cpu[piece->total_drawn] = piece->atlas_mapper[ele_image];
				piece->positions_cpu[piece->total_drawn] = glm::vec3(palete_true_start.x + i, palete_true_start.y, 5);
				piece->movement_cpu[piece->total_drawn] = glm::vec2(0, 0);
				piece->color_cpu[piece->total_drawn] = glm::vec4(1, 1, 1, 1);
				piece->total_drawn++;
			}
		}
	}
}

IntPair calculate_floor_cell_clicked(GameState* currentState,IntPair position, glm::vec2 mouseGamePos)
{

	float left = (float) position.x;
	float right = (float) (position.x + currentState->w);
	float down = (float) position.y;
	float up = (float) (position.y + currentState->h);
	//calculate what floor cell we actually clicked.
	float percentageX = percent_between_two_points(mouseGamePos.x, left, right);
	float percentageY = percent_between_two_points(mouseGamePos.y, down, up);
	int x_floor_cell_clicked = (int)(percentageX * currentState->w);
	int y_floor_cell_clicked = (int)(percentageY * currentState->h);

	return math_intpair_create(x_floor_cell_clicked, y_floor_cell_clicked);
}
bool MaybeApplyBrush(GamestateBrush* palete,int currentBrush, EditorUIState* ui_state, TimeMachineEditor* timeMachine,glm::vec2 mouseGamePos)
{
	for (int i = 0; i < timeMachine->current_number_of_gamestates; i++)
	{
		GameState* currentState = timeMachine->gamestates[i];
		float left = (float) timeMachine->gamestates_positions[i].x;
		float right = (float) (timeMachine->gamestates_positions[i].x + currentState->w);
		float down = (float) timeMachine->gamestates_positions[i].y;
		float up = (float) (timeMachine->gamestates_positions[i].y + currentState->h);
		bool clickedFloor = math_click_is_inside_AABB(left, down, right, up, mouseGamePos.x, mouseGamePos.y);
		if (clickedFloor)
		{
			//calculate what floor cell we actually clicked.
			float percentageX = percent_between_two_points(mouseGamePos.x, left, right);
			float percentageY = percent_between_two_points(mouseGamePos.y, down, up);
			int x_floor_cell_clicked = (int)(percentageX * currentState->w);
			int y_floor_cell_clicked = (int)(percentageY * currentState->h);

			IntPair target_square = math_intpair_create(x_floor_cell_clicked, y_floor_cell_clicked);
			TimeMachineEditorAction action = gamestate_timemachineaction_create_apply_brush(palete[currentBrush], i, target_square);
			//make the application.
			gamestate_timemachine_editor_take_action(timeMachine, NULL, action);
			ui_state->type = ECS_BRUSH;
			return true;
		}
	}
	return false;
}

GameSpaceCamera math_camera_build_for_gamestate(GameState* gamestate, IntPair position, ViewPortCamera viewport, float x_padding, float y_padding)
{
	float ratio = camera_ratio(viewport);
	float start_x = (float) position.x;
	float start_y = (float) position.y;
	float center_x = start_x + ((gamestate->w) / 2.0f);
	float center_y = start_y + ((gamestate->h) / 2.0f);
	float x_require = (gamestate->w + x_padding) / ratio;
	float y_require = gamestate->h + y_padding;
	float camera_height = maxf(x_require, y_require);
	return math_camera_build(camera_height, center_x, center_y, viewport);
}
bool MaybeApplyBrushInPlayMode(Memory* memory, GamestateBrush* palete,int current_brush, EditorUIState* ui_state, GamestateTimeMachine* time_machine, IntPair current_state_pos, glm::vec2 mouseGamePos)
{
	GameState* current_state = &time_machine->state_array[time_machine->num_gamestates_stored - 1];
	GameState* next_state = &time_machine->state_array[time_machine->num_gamestates_stored];
	float left = (float) current_state_pos.x;
	float right = (float) (current_state_pos.x + current_state->w);
	float down = (float) current_state_pos.y;
	float up = (float) (current_state_pos.y + current_state->h);
	bool clickedFloor = math_click_is_inside_AABB(left, down, right, up, mouseGamePos.x, mouseGamePos.y);
	if (clickedFloor)
	{
		//calculate what floor cell we actually clicked.
		float percentageX = percent_between_two_points(mouseGamePos.x, left, right);
		float percentageY = percent_between_two_points(mouseGamePos.y, down, up);
		int x_floor_cell_clicked = (int)(percentageX * current_state->w);
		int y_floor_cell_clicked = (int)(percentageY * current_state->h);

		IntPair target_square = math_intpair_create(x_floor_cell_clicked, y_floor_cell_clicked);
		//ACTUALLY APPLY THE BRUSH TO GAMESTATE.
		gamestate_clone_to_unitialized(current_state, next_state,memory);
		gamestate_apply_brush(next_state, palete[current_brush], x_floor_cell_clicked, y_floor_cell_clicked);
		time_machine->num_gamestates_stored++;
		ui_state->type = ECS_BRUSH;
		return true;
	}
	return false;
}


/*
	
	okay so what are some things I could do that would make me happier:
	2. Code compress where possible
		-we are drawing in 4 different ways, to world edit, view edit, world play, level play. There is alot of overlap.
		-level play, with all its animations, is unecessarily difficult to read. lets fix that.
	3. Architecture
		- mainHelers, while nice, has gotten waaay to big. Let's break those functions into different fill.
*/

/*
	//use curse crate
	//curse crate against wall that needs to stick against wall gets stuck in a "there are two places we are allowed to stand".
	//curse crate s


	Priority:
	1. Editor helper uitilities: nope
	2. Menu -> medium (could be optional)
	3. Animation when player finishes a level -> low
	4. Sound! Sound! Sound! - high
	5. animation for shining -> medium
	6. particle system -> low
	7. research windows conversion - high
	8. ui library -> low
	9. rigidbodies -> no

	4. Sound!
	5. Enscriptem conversion
	1. Convert empty file
	2. Convert example project that relies on sdl2
	3. Convert actual game! 
		3a - probs gonna have to do some file conversion ughf.
*/