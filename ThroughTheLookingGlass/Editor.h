#pragma once
#include "Math.h"
#include "GameState.h"
#include "Constants.h"

enum TimeMachineEditorActionName
{
	TM_APPLY_BRUSH,
	TM_MOVE_GAMESTATE,
	TM_MERGE_GAMESTATES,
	TM_RESIZE_GAMESTATE,
	TM_CREATE_GAMESTATE,
	TM_DELETE_GAMESTATE,
	TM_REPLACE_GAMESTATE,
	TM_UPDATE_GAMESTATE,
	TM_UNDO,
	TM_CHANGE_LEVEL_MODE,
	TM_COUNT
};
enum LevelMode
{
	Crumble,
	Repeat,
	Overworld
};

struct SplitGamestate
{
	int target_gamestate_index;
	int target_x;
	int target_y;
	bool split_row_not_column;
};
struct SurroundWithWalls
{
	int target_gamestate_index;
};
struct ResizeGamestate
{
	int target_gamestate_index;
	IntPair next_starting_position;
	IntPair next_size;
	IntPair displacement_from_input_to_next;
};
struct ApplyBrush
{
	int target_gamestate_index;
	IntPair target_square;
	GamestateBrush brush;
};
struct MoveGamestate
{
	int target_gamestate_index;
	IntPair moveDistance;
};
struct MergeGamestate
{
	int gamestate_index_left;
	int gamestate_index_right;
	IntPair worldspace_left;
	IntPair worldspace_right;
};
struct RemoveGamestate
{
	int gamestate_index;
};
struct CreateGameState {
	int x;
	int y;
	int w;
	int h;
};
struct ReplaceGamestate
{
	GameState* replace_state;
	int index_to_replace;
	char name[GAME_LEVEL_NAME_MAX_SIZE];
};
struct ChangeLevelMode
{
	int index_to_replace;
	LevelMode replace_value;
};
struct UpdateGamestate
{
	GameState* replace_state;
	int index_to_replace;
};
struct TimeMachineEditorAction
{
	TimeMachineEditorActionName action;
	union {
		RemoveGamestate remove;
		MoveGamestate move;
		MergeGamestate merge;
		ApplyBrush brush;
		ResizeGamestate resize;
		CreateGameState create;
		ReplaceGamestate replace;
		UpdateGamestate update;
		ChangeLevelMode level;
	} u;
};

struct LevelName
{
	char name[GAME_LEVEL_NAME_MAX_SIZE];
};


struct TimeMachineEditorStartState
{
	int number_of_gamestates;
	IntPair gamestates_positions[MAX_NUMBER_GAMESTATES];
	GameState* gamestates[MAX_NUMBER_GAMESTATES];
	LevelName names[MAX_NUMBER_GAMESTATES];
	LevelMode modes[MAX_NUMBER_GAMESTATES];
};

struct TimeMachineEditor
{
	Memory* gamestate_memory;
	TimeMachineEditorAction actionList[MAX_EDITOR_ACTIONS];
	IntPair gamestates_positions[MAX_NUMBER_GAMESTATES];
	GameState* gamestates[MAX_NUMBER_GAMESTATES];
	LevelName names[MAX_NUMBER_GAMESTATES];
	LevelMode modes[MAX_NUMBER_GAMESTATES];
	int current_number_of_gamestates;
	int current_number_of_actions;
};

int gamestate_timemachine_get_click_collision(TimeMachineEditor* timeMachine, float mouse_game_pos_x, float mouse_game_pos_y);
TimeMachineEditor* gamestate_timemachine_editor_create(Memory* memory, Memory* GamestateMemory);
void gamestate_timemachine_editor_initialise_from_start(TimeMachineEditor* editor, TimeMachineEditorStartState* pos);
void gamestate_timemachine_startstate_empty_init(TimeMachineEditorStartState* start_state);
void gamestate_timemachine_editor_take_action(TimeMachineEditor* editor, TimeMachineEditorStartState* maybe_start_state, TimeMachineEditorAction action);


/************************GAMESTATE TIME MACHINE**********************/



TimeMachineEditorAction gamestate_timemachineaction_create_undo_action();
TimeMachineEditorAction gamestate_timemachineaction_create_create_action(int xStart, int yStart, int w, int h);
TimeMachineEditorAction gamestate_timemachineaction_create_apply_brush(GamestateBrush brush, int target_gamestate, IntPair target_square);
TimeMachineEditorAction gamestate_timemachineaction_create_resize_gamsestate(int target_gamestate,
	AABB nextPosition,
	IntPair displacement_from_input_to_next);
TimeMachineEditorAction gamestate_timemachineaction_create_move_gamestate(int target_gamestate, IntPair movement);
TimeMachineEditorAction gamestate_timemachineaction_create_delete_gamestate(int target_gamestate);
TimeMachineEditorAction gamestate_timemachineaction_create_replace_gamestate(GameState* replacement, int to_replace, const char* name);
TimeMachineEditorAction gamestate_timemachineaction_create_update_gamestate(GameState* replacement, int to_replace, LevelName name);
TimeMachineEditorAction gamestate_timemachineaction_create_change_level_mode(int target_gamestate, LevelMode next_mode);
