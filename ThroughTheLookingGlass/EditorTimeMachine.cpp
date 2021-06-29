#include "Editor.h"
#include <iostream>


void delete_gamestate_from_list_internal(TimeMachineEditor* editor, int index_to_delete)
{
	editor->world_state.level_state[index_to_delete] = NULL;
	int current_number_of_gamestates = editor->world_state.num_level;
	//delete staircase links.
	for (int i = 0; i < current_number_of_gamestates; i++)
	{
		if (i == index_to_delete)
			continue;
		
		for (int z = 0; z < editor->world_state.level_state[i]->w * editor->world_state.level_state[i]->h;z++)
		{
			//if we find a staircase on this floortile...
			if (editor->world_state.level_state[i]->floor[z] == Floor::F_STAIRCASE || editor->world_state.level_state[i]->floor[z] == F_STAIRCASE_LEVELSTART)
			{
				int tele_link = editor->world_state.level_state[i]->floor_data[z].teleporter_id;
				//and the staircase is pointing to the level we are deleting...
				if (tele_link == index_to_delete)
				{
					editor->world_state.level_state[i]->floor_data[z].teleporter_id = 0;
					editor->world_state.level_state[i]->floor_data[z].teleporter_target_square = math_intpair_create(0, 0);
					editor->world_state.level_state[i]->floor[z] = Floor::F_NONE;
				}
				//and the staircase is pointing to a level that is not been deleted....
				else
				{
					//explination: if the index of the gamestate is greater, then our link decrements by one. 
					if(tele_link > index_to_delete)
						editor->world_state.level_state[i]->floor_data[z].teleporter_id -= 1;
				}
			}
		}
	}
	//move list values down.
	for (int i = index_to_delete; i < current_number_of_gamestates; i++)
	{
		editor->world_state.level_state[i] = editor->world_state.level_state[i + 1];
	}
	for (int i = index_to_delete; i < current_number_of_gamestates; i++)
	{
		editor->world_state.level_position[i] = editor->world_state.level_position[i + 1];
	}
	for (int i = index_to_delete; i < current_number_of_gamestates; i++)
	{
		editor->world_state.level_names[i] = editor->world_state.level_names[i + 1];
	}
	for (int i = index_to_delete; i < current_number_of_gamestates; i++)
	{
		editor->world_state.level_modes[i] = editor->world_state.level_modes[i + 1];
	}
	for (int i = index_to_delete; i < current_number_of_gamestates; i++)
	{
		editor->world_state.level_solved[i] = editor->world_state.level_solved[i + 1];
	}
	editor->world_state.num_level--;
}

TimeMachineEditor* gamestate_timemachine_editor_create(Memory* memory, Memory* gamestate_memory)
{
	std::cout << sizeof(TimeMachineEditor) << std::endl;
	TimeMachineEditor* result = (TimeMachineEditor*) memory_alloc(memory, sizeof(TimeMachineEditor));
	result->gamestate_memory = gamestate_memory;
	result->world_state.num_level = 0;
	result->current_number_of_actions = 0;
	result->world_state.level_state[0] = (GameState*) memory_alloc(gamestate_memory, sizeof(GameState*) * MAX_NUMBER_GAMESTATES);
	for (int i = 0; i < MAX_NUMBER_GAMESTATES ; i++)
		for(int j = 0; j < GAME_LEVEL_NAME_MAX_SIZE;j++)
			result->world_state.level_names[i].name[j] = '\0';
	for(int i = 0; i < MAX_NUMBER_GAMESTATES;i++)
		result->world_state.level_solved[i] = false;
	for(int i = 0; i < MAX_NUMBER_GAMESTATES;i++)
		result->world_state.level_modes[i] = LevelMode::Crumble;
	for(int i = 0; i < MAX_NUMBER_GAMESTATES;i++)
		result->world_state.level_position[i] = math_intpair_create(0,0);
	return result;
}
void gamestate_timemachine_startstate_empty_init(WorldState* start_state)
{
	start_state->num_level = 0;
	for (int i = 0; i < MAX_NUMBER_GAMESTATES; i++)
		for(int j = 0; j < GAME_LEVEL_NAME_MAX_SIZE;j++)
		start_state->level_names[i].name[j] = '\0';
}
void gamestate_timemachine_editor_initialise_from_start(TimeMachineEditor* editor, WorldState* pos)
{
	memory_clear(editor->gamestate_memory);
	//editor->current_number_of_actions = 0; //removed for now, think inits to 0 anyway first time.
	editor->world_state.num_level = pos->num_level;
	for (int i = 0; i < MAX_NUMBER_GAMESTATES; i++)
		editor->world_state.level_names[i] = pos->level_names[i];
	for (int i = 0; i < pos->num_level; i++)
	{
		editor->world_state.level_state[i] = gamestate_clone(pos->level_state[i], editor->gamestate_memory);
	}
	for (int i = 0; i < pos->num_level; i++)
	{
		editor->world_state.level_position[i].x = pos->level_position[i].x;
		editor->world_state.level_position[i].y = pos->level_position[i].y;
	}
	for (int i = 0; i < pos->num_level; i++)
	{
		editor->world_state.level_modes[i] = pos->level_modes[i];
	}
	for (int i = 0; i < pos->num_level; i++)
	{
		editor->world_state.level_solved[i] = pos->level_solved[i];
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
		GameState* gamestate = editor->world_state.level_state[target_gamestate_index];
		IntPair target_brush_square = action.u.brush.target_square;
		int x = target_brush_square.x;
		int y = target_brush_square.y;
		int target_brush_square_1d = f2D(x, y, gamestate->w, gamestate->h);
		GamestateBrush gamestate_brush = action.u.brush.brush;

		bool action_accepted = gamestate_apply_brush(gamestate, gamestate_brush, x, y);
		if (!action_accepted)
		{
			return false;
		}
		else
		{	//if we accept the action, check if we need to update staircase teleporter links, and if we do, update em!
			int num_actions = editor->current_number_of_actions;
			TimeMachineEditorAction	lastAction = editor->actionList[num_actions - 1];
			if (num_actions >= 1 &&
				lastAction.action == TM_APPLY_BRUSH)
			{
				IntPair last_target_square = lastAction.u.brush.target_square;
				int last_target_level = lastAction.u.brush.target_gamestate_index;
				int last_w = editor->world_state.level_state[last_target_level]->w;
				int last_h = editor->world_state.level_state[last_target_level]->h;
				int last_target_square_1d = f2D(last_target_square.x, last_target_square.y, last_w, last_h);
				if (is_staircase(editor->world_state.level_state[last_target_level]->floor[last_target_square_1d]) &&
					is_staircase(editor->world_state.level_state[target_gamestate_index]->floor[target_brush_square_1d]) &&
					editor->world_state.level_state[last_target_level]->floor_data[last_target_square_1d].teleporter_id == -1
					)
				{
						int target_level = action.u.brush.target_gamestate_index;
						IntPair target_square = action.u.brush.target_square;
						int target_w = editor->world_state.level_state[target_level]->w;
						int target_h = editor->world_state.level_state[target_level]->h;
						int target_square_1d = f2D(target_square.x, target_square.y, target_w, target_h);
						editor->world_state.level_state[last_target_level]->floor_data[last_target_square_1d].teleporter_id = target_level;
						
						editor->world_state.level_state[last_target_level]->floor_data[last_target_square_1d].teleporter_target_square = target_square;
						std::cout << "first teleporter id" << target_level << ", target_level" << target_level << std::endl;
						std::cout << "second teleporter id" << last_target_level << ",last target level" << last_target_level << std::endl;
						std::cout << "target square 1d" << target_square_1d << std::endl;
						std::cout << "last target square 1d" << last_target_square_1d << std::endl;
						editor->world_state.level_state[target_level]->floor_data[target_square_1d].teleporter_id = last_target_level;
						editor->world_state.level_state[target_level]->floor_data[target_square_1d].teleporter_target_square = last_target_square;
				}
			}

		}
	}
	else if (action.action == TM_MERGE_GAMESTATES)
	{
		int index_left = action.u.merge.gamestate_index_left;
		int index_right = action.u.merge.gamestate_index_right;
		GameState* left = editor->world_state.level_state[index_left];
		GameState* right = editor->world_state.level_state[index_right];
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
		editor->world_state.level_state[index_left] = next;
		editor->world_state.level_position[index_left] = math_intpair_create(xStart, yStart);

	}
	else if (action.action == TM_MOVE_GAMESTATE)
	{
		int target_gamestate_index = action.u.move.target_gamestate_index;
		IntPair distance = action.u.move.moveDistance;

		editor->world_state.level_position[target_gamestate_index] = math_intpair_add(editor->world_state.level_position[target_gamestate_index], distance);
	}
	else if (action.action == TM_RESIZE_GAMESTATE)
	{
		//TODO: Got to right an actual resize code, basically:
		int w = action.u.resize.next_size.x;
		int h = action.u.resize.next_size.y;
		IntPair displacement = action.u.resize.displacement_from_input_to_next;

		//grab the gamestates a new gamestate with new size.
		int target_gamestate = action.u.resize.target_gamestate_index;
		GameState* old = editor->world_state.level_state[target_gamestate];
		GameState* next = gamestate_create(editor->gamestate_memory, w, h);

		//use a resize function from gamestate.cpp to actually resize the gamestate.
		gamestate_resize(old, next, displacement);

		//update the reference in the editor to use this new resize.
		editor->world_state.level_state[target_gamestate] = next;
		editor->world_state.level_position[target_gamestate] = action.u.resize.next_starting_position;

		//update the links to refer to the new position, not the old position. 
		//wait, how do we do this? 
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

		if (editor->world_state.num_level > MAX_NUMBER_GAMESTATES)
		{
			crash_err("we have overflowed our level editor gamestate memory. Rats! This means you should implement some proper memory techniques to like not have this happen. ");
			//Technique TODO 1: Make a new gamestate memory, copy over all _relevant_ gamestate piece_data to it, and then delete the old gamestate memory.
			//Technique TODO 2: Okay, so rather than been in order, just replace gamestate stuff with malloc/free.
			abort();
		}
		editor->world_state.level_state[editor->world_state.num_level] = next;
		editor->world_state.level_position[editor->world_state.num_level] = bottom_left;
		editor->world_state.num_level++;
	}
	else if (action.action == TM_REPLACE_GAMESTATE)
	{
		int pos = action.u.replace.index_to_replace;
		GameState* state = action.u.replace.replace_state;
		editor->world_state.level_state[pos] = state;
		for (int i = 0; i < GAME_LEVEL_NAME_MAX_SIZE; i++)
		{
			editor->world_state.level_names[pos].name[i] = action.u.replace.name[i];
		}
	}
	else if (action.action == TM_UPDATE_GAMESTATE)
	{
		int pos = action.u.update.index_to_replace;
		GameState* state = action.u.update.replace_state;
		editor->world_state.level_state[pos] = state;
	}
	else if (action.action == TM_CHANGE_LEVEL_MODE)
	{
		editor->world_state.level_modes[action.u.level.index_to_replace] = action.u.level.replace_value;
	}
	return true; //we accept action.
}
void gamestate_timemachine_editor_take_action(TimeMachineEditor* editor, WorldState* maybe_start_state, TimeMachineEditorAction action)
{
	//if we are using a meta action, perform meta action. Otherwise, add action to list of actions.
	if (action.action == TM_UNDO && editor->current_number_of_actions > 0)
	{
		editor->current_number_of_actions--;

		//reset the whole state to the beginning.
		if (!maybe_start_state)
			crash_err("hmm, when doing an undo action, we passed, a NULL when we should have given a pos state. Fix it!");
		gamestate_timemachine_editor_initialise_from_start(editor, maybe_start_state);
		for (int i = 0; i < MAX_NUMBER_GAMESTATES; i++)
			for(int j = 0; j < GAME_LEVEL_NAME_MAX_SIZE;j++)
			editor->world_state.level_names[i].name[j] = '\0';
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
	for (int i = 0; i < timeMachine->world_state.num_level; i++)
	{
		GameState* currentState = timeMachine->world_state.level_state[i];
		float left = (float) timeMachine->world_state.level_position[i].x;
		float right = (float) timeMachine->world_state.level_position[i].x + currentState->w;
		float down = (float) timeMachine->world_state.level_position[i].y;
		float up = (float) timeMachine->world_state.level_position[i].y + currentState->h;
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
TimeMachineEditorAction gamestate_timemachineaction_create_update_gamestate(GameState* replacement, int to_replace, LevelName name)
{
	TimeMachineEditorAction action;
	action.action = TM_UPDATE_GAMESTATE;
	action.u.update.replace_state = replacement;
	action.u.update.index_to_replace = to_replace;
	for (int i = 0; i < GAME_LEVEL_NAME_MAX_SIZE; i++)
	{
		action.u.replace.name[i] = name.name[i];
	}
	return action;
}
TimeMachineEditorAction gamestate_timemachineaction_create_change_level_mode(int target_gamestate, LevelMode next_mode)
{
	TimeMachineEditorAction action;
	action.action = TM_CHANGE_LEVEL_MODE;
	action.u.level.index_to_replace = target_gamestate;
	action.u.level.replace_value = next_mode;
	return action;
}
