#include "Editor.h"
#include <iostream>

void delete_gamestate_from_list_internal(TimeMachineEditor* editor, int index_to_delete)
{
	editor->gamestates[index_to_delete] = NULL;
	int current_number_of_gamestates = editor->current_number_of_gamestates;
	for (int i = index_to_delete; i < current_number_of_gamestates; i++)
	{
		editor->gamestates[i] = editor->gamestates[i + 1];
	}
	for (int i = index_to_delete; i < current_number_of_gamestates; i++)
	{
		editor->gamestates_positions[i] = editor->gamestates_positions[i + 1];
	}
	editor->current_number_of_gamestates--;
}

TimeMachineEditor* gamestate_timemachine_editor_create(Memory* memory, Memory* gamestate_memory)
{
	std::cout << sizeof(TimeMachineEditor) << std::endl;
	TimeMachineEditor* result = (TimeMachineEditor*) memory_alloc(memory, sizeof(TimeMachineEditor));
	result->gamestate_memory = gamestate_memory;
	result->current_number_of_gamestates = 0;
	result->current_number_of_actions = 0;
	result->gamestates[0] = (GameState*) memory_alloc(gamestate_memory, sizeof(GameState*) * MAX_NUMBER_GAMESTATES);
	for (int i = 0; i < MAX_NUMBER_GAMESTATES * GAME_LEVEL_NAME_MAX_SIZE; i++)
		result->names[i] = '\0';
	return result;
}
void gamestate_timemachine_startstate_empty_init(TimeMachineEditorStartState* start_state)
{
	start_state->number_of_gamestates = 0;
	for (int i = 0; i < MAX_NUMBER_GAMESTATES * GAME_LEVEL_NAME_MAX_SIZE; i++)
		start_state->names[i] = '\0';
}
void gamestate_timemachine_editor_initialise_from_start(TimeMachineEditor* editor, TimeMachineEditorStartState* pos)
{
	memory_clear(editor->gamestate_memory);
	//editor->current_number_of_actions = 0; //removed for now, think inits to 0 anyway first time.
	editor->current_number_of_gamestates = pos->number_of_gamestates;
	for (int i = 0; i < GAME_LEVEL_NAME_MAX_SIZE * MAX_NUMBER_GAMESTATES; i++)
		editor->names[i] = pos->names[i];
	for (int i = 0; i < pos->number_of_gamestates; i++)
	{
		editor->gamestates[i] = gamestate_clone(pos->gamestates[i], editor->gamestate_memory);
	}
	for (int i = 0; i < pos->number_of_gamestates; i++)
	{
		editor->gamestates_positions[i].x = pos->gamestates_positions[i].x;
		editor->gamestates_positions[i].y = pos->gamestates_positions[i].y;
	}
}
bool take_unlogged_action(TimeMachineEditor* editor, TimeMachineEditorAction action)
{
	if (action.action == TM_DELETE_GAMESTATE)
	{
		int index_to_delete = action.u.remove.gamestate_index;
		delete_gamestate_from_list_internal(editor, index_to_delete);
	}
	if (action.action == TM_APPLY_BRUSH)
	{
		int target_gamestate_index = action.u.brush.target_gamestate_index;
		GameState* gamestate = editor->gamestates[target_gamestate_index];
		IntPair target_brush_square = action.u.brush.target_square;
		int x = target_brush_square.x;
		int y = target_brush_square.y;
		GamestateBrush gamestate_brush = action.u.brush.brush;


		bool action_accepted = gamestate_apply_brush(gamestate, gamestate_brush, x, y);
		if (!action_accepted)
		{
			return false;
		}
	}
	else if (action.action == TM_MERGE_GAMESTATES)
	{
		int index_left = action.u.merge.gamestate_index_left;
		int index_right = action.u.merge.gamestate_index_right;
		GameState* left = editor->gamestates[index_left];
		GameState* right = editor->gamestates[index_right];
		IntPair worldspace_left = action.u.merge.worldspace_left;
		IntPair worldspace_right = action.u.merge.worldspace_right;

		int xStart = mini(worldspace_left.x, worldspace_right.x);
		int yStart = mini(worldspace_left.y, worldspace_right.y);
		IntPair leftOffset = math_intpair_create(worldspace_left.x - xStart, worldspace_left.y - yStart);
		IntPair rightOffset = math_intpair_create(worldspace_right.x - xStart, worldspace_right.y - yStart);

		int output_length_x = maxi(leftOffset.x + left->w, rightOffset.x + right->w);
		int output_length_y = maxi(leftOffset.y + left->h, rightOffset.y + right->h);
		IntPair output_length = math_intpair_create(output_length_x, output_length_y);
		GameState* next = gamestate_merge_with_allocate(left, right, output_length, editor->gamestate_memory, leftOffset, rightOffset);

		delete_gamestate_from_list_internal(editor, index_right);
		editor->gamestates[index_left] = next;
		editor->gamestates_positions[index_left] = math_intpair_create(xStart, yStart);

	}
	else if (action.action == TM_MOVE_GAMESTATE)
	{
		int target_gamestate_index = action.u.move.target_gamestate_index;
		IntPair distance = action.u.move.moveDistance;

		editor->gamestates_positions[target_gamestate_index] = math_intpair_add(editor->gamestates_positions[target_gamestate_index], distance);
	}
	else if (action.action == TM_RESIZE_GAMESTATE)
	{
		//TODO: Got to right an actual resize code, basically:
		int w = action.u.resize.next_size.x;
		int h = action.u.resize.next_size.y;
		IntPair displacement = action.u.resize.displacement_from_input_to_next;

		//grab the gamestates a new gamestate with new size.
		int target_gamestate = action.u.resize.target_gamestate_index;
		GameState* old = editor->gamestates[target_gamestate];
		GameState* next = gamestate_create(editor->gamestate_memory, w, h);

		//use a resize function from gamestate.cpp to actually resize the gamestate.
		gamestate_resize(old, next, displacement);

		//update the reference in the editor to use this new resize.
		editor->gamestates[target_gamestate] = next;
		editor->gamestates_positions[target_gamestate] = action.u.resize.next_starting_position;
	}
	else if (action.action == TM_CREATE_GAMESTATE)
	{
		//create the gamestate.
		int x = action.u.create.x;
		int y = action.u.create.y;
		IntPair bottom_left = math_intpair_create(x, y);
		int w = action.u.create.w;
		int h = action.u.create.h;

		GameState* next = gamestate_create(editor->gamestate_memory, w, h);

		//add the gamestate (and its position on the world) to the editor.

		if (editor->current_number_of_gamestates > MAX_NUMBER_GAMESTATES)
		{
			crash_err("we have overflowed our level editor gamestate memory. Rats! This means you should implement some proper memory techniques to like not have this happen. ");
			//Technique TODO 1: Make a new gamestate memory, copy over all _relevant_ gamestate piece_data to it, and then delete the old gamestate memory.
			//Technique TODO 2: Okay, so rather than been in order, just replace gamestate stuff with malloc/free.
			abort();
		}
		editor->gamestates[editor->current_number_of_gamestates] = next;
		editor->gamestates_positions[editor->current_number_of_gamestates] = bottom_left;
		editor->current_number_of_gamestates++;
	}
	else if (action.action == TM_REPLACE_GAMESTATE)
	{
		int pos = action.u.replace.index_to_replace;
		GameState* state = action.u.replace.replace_state;
		editor->gamestates[pos] = state;
		for (int i = 0; i < GAME_LEVEL_NAME_MAX_SIZE; i++)
		{
			editor->names[pos * GAME_LEVEL_NAME_MAX_SIZE + i] = action.u.replace.name[i];
		}
	}
	else if (action.action == TM_UPDATE_GAMESTATE)
	{
		int pos = action.u.update.index_to_replace;
		GameState* state = action.u.update.replace_state;
		editor->gamestates[pos] = state;
	}
	return true; //we accept action.
}
void gamestate_timemachine_editor_take_action(TimeMachineEditor* editor, TimeMachineEditorStartState* maybe_start_state, TimeMachineEditorAction action)
{
	//if we are using a meta action, perform meta action. Otherwise, add action to list of actions.
	if (action.action == TM_UNDO && editor->current_number_of_actions > 0)
	{
		editor->current_number_of_actions--;

		//reset the whole state to the beginning.
		if (!maybe_start_state)
			crash_err("hmm, when doing an undo action, we passed, a NULL when we should have given a pos state. Fix it!");
		gamestate_timemachine_editor_initialise_from_start(editor, maybe_start_state);
		for (int i = 0; i < MAX_NUMBER_GAMESTATES * GAME_LEVEL_NAME_MAX_SIZE; i++)
			editor->names[i] = '\0';
		for (int i = 0; i < editor->current_number_of_actions; i++)
		{
			take_unlogged_action(editor, editor->actionList[i]);
		}
	}
	if(action.action != TM_UNDO)
	{
		bool accepted_action = take_unlogged_action(editor, action);
		if (accepted_action)
		{
			editor->actionList[editor->current_number_of_actions] = action;
			editor->current_number_of_actions++;
		}
	}

}


int gamestate_timemachine_get_click_collision(TimeMachineEditor* timeMachine, float mouse_game_pos_x,float mouse_game_pos_y)
{
	for (int i = 0; i < timeMachine->current_number_of_gamestates; i++)
	{
		GameState* currentState = timeMachine->gamestates[i];
		float left = (float) timeMachine->gamestates_positions[i].x;
		float right = (float) timeMachine->gamestates_positions[i].x + currentState->w;
		float down = (float) timeMachine->gamestates_positions[i].y;
		float up = (float) timeMachine->gamestates_positions[i].y + currentState->h;
		bool clickedFloor = math_click_is_inside_AABB(left, down, right, up, mouse_game_pos_x, mouse_game_pos_y);
		if(clickedFloor)
			return i;
	}
	return -1;
}
/*****************************************************action creation*******************************************************/
TimeMachineEditorAction gamestate_timemachineaction_create_undo_action()
{
	TimeMachineEditorAction action;
	action.action = TM_UNDO;
	return action;
}
TimeMachineEditorAction gamestate_timemachineaction_create_create_action(int x, int y, int w, int h)
{
	TimeMachineEditorAction action;
	action.action = TM_CREATE_GAMESTATE;
	action.u.create.x = x;
	action.u.create.y = y;
	action.u.create.w = w;
	action.u.create.h = h;
	return action;
}
TimeMachineEditorAction gamestate_timemachineaction_create_apply_brush(GamestateBrush brush, int target_gamestate, IntPair target_square)
{
	TimeMachineEditorAction action;
	action.action = TM_APPLY_BRUSH;
	action.u.brush.brush = brush;
	action.u.brush.target_gamestate_index = target_gamestate;
	action.u.brush.target_square = target_square;
	return action;
}
TimeMachineEditorAction gamestate_timemachineaction_create_resize_gamsestate(int target_gamestate, 
	AABB nextPosition,
	IntPair displacement_from_input_to_next)
{
	TimeMachineEditorAction action;
	action.action = TM_RESIZE_GAMESTATE;
	action.u.resize.next_starting_position.x = (int) nextPosition.x;
	action.u.resize.next_starting_position.y = (int) nextPosition.y;
	action.u.resize.target_gamestate_index = target_gamestate;
	action.u.resize.displacement_from_input_to_next = displacement_from_input_to_next;
	action.u.resize.next_size.x = (int) nextPosition.w;
	action.u.resize.next_size.y = (int) nextPosition.h;
	return action;
}
TimeMachineEditorAction gamestate_timemachineaction_create_move_gamestate(int target_gamestate, IntPair movement)
{
	TimeMachineEditorAction action;
	action.action = TM_MOVE_GAMESTATE;
	action.u.move.moveDistance = movement;
	action.u.move.target_gamestate_index = target_gamestate;
	return action;
}
TimeMachineEditorAction gamestate_timemachineaction_create_delete_gamestate(int target_gamestate)
{
	TimeMachineEditorAction action;
	action.action = TM_DELETE_GAMESTATE;
	action.u.remove.gamestate_index = target_gamestate;
	return action;
}

TimeMachineEditorAction gamestate_timemachineaction_create_replace_gamestate(GameState* replacement, int to_replace, const char* name)
{
	TimeMachineEditorAction action;
	action.action = TM_REPLACE_GAMESTATE;
	action.u.replace.replace_state = replacement;
	action.u.replace.index_to_replace = to_replace;
	for (int i = 0; i < GAME_LEVEL_NAME_MAX_SIZE; i++)
	{
		action.u.replace.name[i] = name[i];
	}
	return action;
}
TimeMachineEditorAction gamestate_timemachineaction_create_update_gamestate(GameState* replacement, int to_replace, const char* name)
{
	TimeMachineEditorAction action;
	action.action = TM_UPDATE_GAMESTATE;
	action.u.update.replace_state = replacement;
	action.u.update.index_to_replace = to_replace;
	for (int i = 0; i < GAME_LEVEL_NAME_MAX_SIZE; i++)
	{
		action.u.replace.name[i] = name[i];
	}
	return action;
}