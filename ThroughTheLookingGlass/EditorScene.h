#pragma once
#include "GameState.h"
#include "Editor.h"

const int palete_length = 11;
struct EditorScene
{
	TimeMachineEditor* timeMachine;
	TimeMachineEditorStartState* time_machine_start_state;
	int currentBrush = 0;
	GamestateBrush* palete;
	IntPair palete_screen_start;
};
//result->time_machine_start_state = (TimeMachineEditorStartState *) memory_alloc(editor_memory,sizeof(TimeMachineEditorStartState));

EditorScene* editorscene_setup_with_start_state(Memory* editor_memory, ViewPortCamera camera_viewport, TimeMachineEditorStartState* start_state)
{
	EditorScene* result = (EditorScene*)memory_alloc(editor_memory, sizeof(EditorScene));
	result->timeMachine = gamestate_timemachine_editor_create(editor_memory, memory_create(10000000));
	result->time_machine_start_state = start_state;
	gamestate_timemachine_editor_initialise_from_start(result->timeMachine, result->time_machine_start_state);

	result->palete = (GamestateBrush*)memory_alloc(editor_memory, sizeof(GamestateBrush) * palete_length);
	result->currentBrush = 0;
	result->palete_screen_start = math_intpair_create(camera_viewport.left + 60, camera_viewport.up - 120);
	{
		int i = 0;
		result->palete[i++] = gamestate_brush_create(true, F_NONE, true, P_NONE);
		result->palete[i++] = gamestate_brush_create(true, F_TARGET, false, P_NONE);
		result->palete[i++] = gamestate_brush_create(true, F_START, false, P_NONE);
		result->palete[i++] = gamestate_brush_create(true, F_LURKING_WALL, false, P_NONE);
		result->palete[i++] = gamestate_brush_create(false, F_NONE, true, Piece::P_NONE);
		result->palete[i++] = gamestate_brush_create(false, F_NONE, true, P_WALL);
		result->palete[i++] = gamestate_brush_create(false, F_NONE, true, P_WALL_ALT);
		result->palete[i++] = gamestate_brush_create(false, F_NONE, true, P_CRUMBLE);
		result->palete[i++] = gamestate_brush_create(false, F_NONE, true, P_CRATE);
		result->palete[i++] = gamestate_brush_create(false, F_NONE, true, P_PLAYER);
		if (i != palete_length)
		{
			crash_err("you need the number of elements in the palete to be teh same as the length of the palete");
		}
			
		return result;
	}
}
EditorScene* editorscene_setup(Memory* editor_memory, ViewPortCamera camera_viewport)
{
	memory_clear(editor_memory);
	TimeMachineEditorStartState* pass = (TimeMachineEditorStartState*)memory_alloc(editor_memory, sizeof(TimeMachineEditorStartState));
	gamestate_timemachine_startstate_empty_init(pass);
	return editorscene_setup_with_start_state(editor_memory, camera_viewport, pass);
}
