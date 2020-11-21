#pragma once

#include "Constants.h"
#include "Memory.h"
#include "Math.h"
#include <iostream>
enum LAYER_NAME
{
	LN_FLOOR,
	LN_PIECE,
	LN_COUNT
};

const int GAME_NUM_LAYERS = LN_COUNT; 
const int PLAYER_LAYER = LN_PIECE; 

enum Piece {
	P_NONE,
	P_PLAYER,
	P_CRATE,
	P_PULL_CRATE,
	P_CRUMBLE,
	P_WALL,
	P_WALL_ALT,
	P_COUNT
};

enum Floor {
	F_NONE,
	F_TARGET,
	F_OUTLINE,
	F_START,
	F_ZBLACK,
	F_LURKING_WALL,
	F_COUNT
};
struct GameState {
	int w;
	int h;
	int* floor;
	int* piece;
};

struct GamestateBrush {
	bool applyFloor;
	bool applyPiece;
	Floor floor;
	Piece piece;
};

struct GamestateTimeMachine
{
	GameState* start_state;
	GameState* state_array;
	int num_gamestates_stored;
	int max_gamestates_storable;
};
enum Action {
	A_UP,
	A_RIGHT,
	A_DOWN,
	A_LEFT,
	A_UNDO,
	A_RESET,
	A_NONE,
	A_COUNT
};


enum AnimationType {
	AT_NONE,
	AT_MOVE,
	AT_COUNT
};

struct PieceMovementAnimation
{
	IntPair* pos;
	Direction* to_move;
	int* start_value;
};
struct GameStateAnimation
{
	int num_to_draw;
	PieceMovementAnimation starts;
	PieceMovementAnimation ends;
};
enum ActionResult
{
	AR_ACTION_CANCELLED,
	AR_ACTION_OCCURED,
	AR_COUNT
};
struct GameActionJournal
{
	GameState* old_state;
	ActionResult action_result;
	GameStateAnimation* maybe_animation;
};
Direction action_to_direction(Action action);
IntPair direction_to_intpair(Direction dir);
void gamestate_timemachine_reset(GamestateTimeMachine* timeMachine, Memory* scope_memory);

/*****************************GAMESTATE TIME MACHINE******************/
/*********************************************************************/
void gamestate_timemachine_reset(GamestateTimeMachine* timeMachine, Memory* scope_memory);
void* gamestate_timemachine_undo(GamestateTimeMachine* timeMachine);
GameState* gamestate_timemachine_get_latest_gamestate(GamestateTimeMachine* timeMachine);
GamestateTimeMachine* gamestate_timemachine_create(GameState* start_state, Memory* memory, int max_num_gamestates);
GameActionJournal* gamestate_timemachine_take_action(GamestateTimeMachine* timeMachine, Direction action, Memory* scope_memory, Memory* temp_memory);
/*************************PALETTE_BRUSH*******************************/
/*********************************************************************/
GamestateBrush gamestate_brush_create(bool applyFloor, Floor floor, bool applyPiece, Piece piece);

/******************************GAMESTATE WRITE************************/
/*********************************************************************/

GameState* gamestate_create(Memory* memory, int w, int h);
GameState* gamestate_clone(GameState* state, Memory* memory);
void gamestate_clone_to_unitialized(GameState* input, GameState* output, Memory* memory);
void  gamestate_clone_to(GameState* input_state, GameState* output_state);

GameState* gamestate_add_row(GameState* input, Memory* scope_memory, int target_x, int target_y);
GameState* gamestate_add_column(GameState* input, Memory* scope_memory, int target_x, int target_y);
GameState* gamestate_surround_with_walls(GameState* input, Memory* scope_memory);
void gamestate_resize(GameState* input_state, GameState* output_state, IntPair displacement_from_input_to_output);
GameState* gamestate_resize_with_allocate(GameState* input_state, Memory* output_memory, int output_w, int output_h, IntPair displacement_from_input_to_output);
bool gamestate_apply_brush(GameState* state, GamestateBrush brush, int x, int y);
GameState* gamestate_merge_with_allocate(GameState* first, GameState* second, IntPair combined_size, Memory* output_memory, IntPair left_merge_offset, IntPair right_merge_offset);
void gamestate_merge(GameState* left, GameState* right, GameState* output, IntPair left_merge_offset, IntPair right_merge_offset);
/******************************GAMESTATE READ************************/
/********************************************************************/
int** gamestate_get_layers(GameState* gamestate, int* num_layers_found, Memory* temp_memory);
int* gamestate_get_layer(GameState* gamestate, int layer_num);
bool gamestate_eq(GameState* left, GameState* right);
//unsigned int gamestate_player_find(GameState* gamestate);
AABB* gamestate_create_colliders(Memory* memory, GameState** states, IntPair* locations, int length);
AABB* gamestate_create_colliders(Memory* memory, GameState** states, IntPair* locations, int length, int skip_index);
bool gamestate_is_in_win_condition(GameState* state);


bool is_player(int val);
bool is_normal_crate(int val);
bool is_pull_crate(int val);

/******************************GAMESTATE GAME ACTION WRITE**********************/
/********************************************************************/

GameActionJournal* gamestate_action(GameState* state, Direction action, Memory* temp_memory);
void gamestate_crumble(GameState* state);
void gamestate_extrude_lurking_walls(GameState* state);
