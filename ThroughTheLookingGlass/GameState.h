#pragma once

#include "Constants.h"
#include "Memory.h"
#include "Math.h"
#include <iostream>
#include "textureAssets.h"
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
	F_GRILL_COLD,
	F_GRILL_HOT,
	F_EXIT,
	F_STAIRCASE,
	F_STAIRCASE_LEVELSTART,
	F_STAIRCASE_SOLVED,
	F_COUNT
};
enum CratePower {
	CP_PUSH,
	CP_PULL,
	CP_PARALLEL,
	CP_MERGE,
	CP_STICKY,
	CP_COUNT,
	CP_ERROR
};
struct PieceData
{
	bool powers[CP_COUNT];
};

struct Teleporter_Location;
struct FloorData
{
	//target match rules.
	int teleporter_id; //if we teleporting, which of our levels rules we want to refer to.
	IntPair teleporter_target_square;
};

struct GameState {
	int w;
	int h;
	int* floor;
	int* piece;
	FloorData* floor_data;
	PieceData* piece_data;
};

struct GamestateBrush {
	bool applyFloor;
	bool applyPiece;
	Floor floor;
	Piece piece;
	bool applyCratePower;
	PieceData piece_data;
	bool applyFloorPower;
	FloorData floor_data;
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


struct SymbolGlobalAnimation
{
	int len;
	IntPair* pos; //where we are going to end up drawing after animation.
	Direction* to_move; //which direciton we are moving to our draw position from.
	int* img_value; //what image we are drawing.
};
struct SymbolAnimation
{
	int len;
	int* index;
	IntPair* global_position;
	Direction* global_move;
	PieceData* start_powers;	
	PieceData* end_powers;	//when the crate has finished moving, what powers will be in effect? 
	PieceData* flash_powers;	//true if should be flashed to indicate its effect is occuring.
};
struct MaskedMovementAnimation
{
	IntPair* pos; //where we are going to end up drawing after animation.
	Direction* to_move; //which direciton we are moving to our draw position from.
	//implicit; mask is POS. 
	int* img_value; //what image we are drawing.
};
struct GameStateAnimation
{
	int num_to_draw;
	//for drawing the crates moving.
	MaskedMovementAnimation starts; 
	MaskedMovementAnimation ends; 
	//for drawing the symbols inside the crates moving locally.
	SymbolAnimation symbol_start;
	SymbolAnimation symbol_end;
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

struct RigidData
{
	int* grid;
	int num_rigids;
	IntPair* links;
	int num_links;
};

struct WorldPosition
{
	int level_index;
	IntPair level_position;
	int level_position_1d;
};

Direction action_to_direction(Action action);
IntPair direction_to_intpair(Direction dir);
void gamestate_timemachine_reset(GamestateTimeMachine* timeMachine, Memory* scope_memory);

WorldPosition world_make_world_position(int level_index, IntPair pos_2d, int pos_1d);
/*****************************GAMESTATE TIME MACHINE******************/
/*********************************************************************/
void gamestate_timemachine_reset(GamestateTimeMachine* timeMachine, Memory* scope_memory);
void* gamestate_timemachine_undo(GamestateTimeMachine* timeMachine);
GameState* gamestate_timemachine_get_latest_gamestate(GamestateTimeMachine* timeMachine);
GamestateTimeMachine* gamestate_timemachine_create(GameState* start_state, Memory* memory, int max_num_gamestates);
GameActionJournal* gamestate_timemachine_take_action(GamestateTimeMachine* timeMachine, Direction action, Memory* scope_memory, Memory* temp_memory);
/*************************PALETTE_BRUSH*******************************/
/*********************************************************************/
GamestateBrush gamestate_brush_create(bool applyFloor, Floor floor, bool applyPiece, Piece piece, bool applyPieceData, PieceData pieceData, bool applyFloorData, FloorData floorData);

/*************************PIECE/FLOOR DATA*******************************/
/*********************************************************************/
textureAssets::SYMBOLS piecedata_to_symbol(CratePower val);
PieceData gamestate_piecedata_make();
PieceData gamestate_piecedata_make(CratePower val);
PieceData gamestate_piecedata_make(CratePower val, CratePower val2);
PieceData gamestate_piecedata_make(CratePower val, CratePower val2,CratePower val3);	
FloorData gamestate_floordata_make();
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
int gamestate_calc_clicked_state(GameState** states, IntPair* states_positions, int num_states, float mouse_game_pos_x, float mouse_game_pos_y);
WorldPosition gamestate_calc_clicked_world_position(GameState** states, IntPair* states_positions, int num_states, float mouse_game_pos_x, float mouse_game_pos_y);
WorldPosition gamestate_get_position_linked_by_teleporter(GameState** gamestates, int len, WorldPosition first_staircase);
WorldPosition gamestate_get_position_linked_by_teleporter_and_check_backlink(GameState** gamestates, int len, WorldPosition first_staircase);
void gamestate_print_staircase_tele_value(GameState** gamestate,int len);
int** gamestate_get_layers(GameState* gamestate, int* num_layers_found, Memory* temp_memory);
int* gamestate_get_layer(GameState* gamestate, int layer_num);
//unsigned int gamestate_player_find(GameState* gamestate);
AABB* gamestate_create_colliders(Memory* memory, GameState** states, IntPair* locations, int length);
AABB* gamestate_create_colliders(Memory* memory, GameState** states, IntPair* locations, int length, int skip_index);
bool gamestate_is_in_win_condition(GameState* state);

int gamestate_maybe_find_player(GameState* state);
int maybe_find_exit_or_return_neg1(GameState* state);
bool is_level_start(int val);
bool is_staircase(int val);
bool is_player(int val);

glm::vec2 piecedata_calculate_scale(PieceData piece);
void piecedata_calculate_local_positions(PieceData piece, glm::vec2* scale, glm::vec2* out_positions);

/******************************GAMESTATE GAME ACTION WRITE**********************/
/********************************************************************/

GameActionJournal* gamestate_action(GameState* state, Direction action, Memory* temp_memory);

void gamestate_startup(GameState* state);
void gamestate_crumble(GameState* state);
void gamestate_extrude_lurking_walls(GameState* state);
