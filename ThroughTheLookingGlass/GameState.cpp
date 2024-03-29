#include "GameState.h"
#include "Memory.h"
#include <iostream>
#include "Math.h"

/*INTERNAL FUNCTIONS HEADERS*/
void gamestate_allocate_layers(GameState* result, Memory* memory, int w, int h);


/**************************WORLD POSITION FUNCS***********************************/
WorldPosition world_make_world_position(int level_index, IntPair pos_2d, int pos_1d)
{
	WorldPosition result;
	result.level_index = level_index;
	result.level_position = pos_2d;
	result.level_position_1d = pos_1d;
	return result;
}
/***************************MOVEMENT FUNCTIONS***********************************/
static inline IntPair move_pos_wrapped_2d_up(IntPair pos, int w, int h)
{
	pos.y += 1;
	pos.y -= (pos.y >= h) * h;
	return pos;
}
static inline IntPair move_pos_wrapped_2d_right(IntPair pos, int w, int h) {
	pos.x += 1;
	pos.x -= (pos.x >= w) * w;
	return pos;
}
static inline IntPair move_pos_wrapped_2d_down(IntPair pos, int w, int h) {
	pos.y -= 1;
	pos.y += (pos.y < 0) * h;
	return pos;
}
static inline IntPair move_pos_wrapped_2d_left(IntPair pos, int w, int h) {
	pos.x -= 1;
	pos.x += (pos.x < 0) * w;
	return pos;
}
static inline IntPair move_pos_wrapped_2d(IntPair pos, Direction direction, int w, int h)
{
	switch (direction) {
	case U:
		pos.y += 1;
		pos.y -= (pos.y >= h) * h;
		return pos;
	case R:
		pos.x += 1;
		pos.x -= (pos.x >= w) * w;
		return pos;
	case D:
		pos.y -= 1;
		pos.y += (pos.y < 0) * h;
		return pos;
	case L:
		pos.x -= 1;
		pos.x += (pos.x < 0) * w;
		return pos;
	}
	return pos;
}

template <class T>
static void apply_moves_to_layer_SLOW(Memory* temp_memory, bool* is_moving, T* layer, int w, int h, Direction action, T empty_val)
{
	//TODO: No longer need to call this, replace call with faster code, holy moly this is slow.
	int length = w * h;
	T* next_layer = (T*) memory_alloc(temp_memory, sizeof(T) * length);
	int* shall_delete = (int*)memory_alloc(temp_memory, sizeof(int) * length);
	for (int z = 0; z < length; z++)
	{
		shall_delete[z] = 0;
	}
	for (int z = 0; z < length; z++)
	{
		next_layer[z] = layer[z];
	}
	for (int z = 0; z < length; z++)
	{
		IntPair p = t2D(z, w, h);
		IntPair next = move_pos_wrapped_2d(p, action, w, h);
		int next_1d = f2D(next.x, next.y, w, h);
		if (is_moving[z])
		{
			next_layer[next_1d] = layer[z];
			shall_delete[z]++;
			shall_delete[next_1d]--;
		}
	}

	for (int z = 0; z < length; z++)
	{
		if (shall_delete[z] > 0)
			next_layer[z] = empty_val;
	}
	for (int z = 0; z < length; z++)
	{
		layer[z] = next_layer[z];
	}
}
static void apply_left_moves_to_layer(Memory* memory, bool* is_moving, int* layer, int w, int h)
{
	int length = w * h;
	for (int j = 0; j < h; j++)
	{
		int final_i = f2D(w - 1, j, w, h);
		int stored_value = layer[final_i];

		for (int i = 0; i < w - 1; i++)
		{
			int pos = f2D(i, j, w, h);
			IntPair next = move_pos_wrapped_2d_left(math_intpair_create(i, j), w, h);
			int next_1d = f2D(next.x, next.y, w, h);
			layer[next_1d] = (layer[pos] * is_moving[pos]) + (layer[next_1d] * (1 - is_moving[pos]));
		}
		IntPair next_final_2d = move_pos_wrapped_2d_left(math_intpair_create(w - 1, j), w, h);
		int next_final_1d = f2D(next_final_2d.x, next_final_2d.y, w, h);
		layer[next_final_1d] = (stored_value * is_moving[final_i]) + (layer[next_final_1d] * (1 - is_moving[final_i]));
	}
}
static void apply_right_moves_to_layer(Memory* memory, bool* is_moving, int* layer, int w, int h)
{
	int length = w * h;
	int z = 0;
	for (int j = 0; j < h; j++)
	{
		int final_i = f2D(0, j, w, h);
		int stored_value = layer[final_i];

		for (int i = w - 1; i >= 0; i--)
		{
			int pos = f2D(i, j, w, h);
			IntPair next = move_pos_wrapped_2d_right(math_intpair_create(i, j), w, h);
			int next_1d = f2D(next.x, next.y, w, h);
			layer[next_1d] = (layer[pos] * is_moving[pos]) + (layer[next_1d] * (1 - is_moving[pos]));
		}

		IntPair next_final_2d = move_pos_wrapped_2d_right(math_intpair_create(0, j), w, h);
		int next_final_1d = f2D(next_final_2d.x, next_final_2d.y, w, h);
		layer[next_final_1d] = (stored_value * is_moving[final_i]) + (layer[next_final_1d] * (1 - is_moving[final_i]));
	}
}
static void apply_up_moves_to_layer(Memory* memory, bool* is_moving, int* layer, int w, int h)
{
	int length = w * h;
	for (int i = 0; i < w; i++)
	{
		int final_i = f2D(i, h - 1, w, h);
		int stored_value = layer[final_i];

		for (int j = 1; j < h; j++)
		{
			int pos = f2D(i, j, w, h);
			IntPair next = move_pos_wrapped_2d_up(math_intpair_create(i, j), w, h);
			int next_1d = f2D(next.x, next.y, w, h);
			layer[next_1d] = (layer[pos] * is_moving[pos]) + (layer[next_1d] * (1 - is_moving[pos]));
		}

		IntPair next_final_2d = move_pos_wrapped_2d_up(math_intpair_create(i, h - 1), w, h);
		int next_final_1d = f2D(next_final_2d.x, next_final_2d.y, w, h);
		layer[next_final_1d] = (stored_value * is_moving[final_i]) + (layer[next_final_1d] * (1 - is_moving[final_i]));
	}
}
static void apply_down_moves_to_layer(Memory* memory, bool* is_moving, int* layer, int w, int h)
{
	int length = w * h;
	for (int i = 0; i < w; i++)
	{
		int final_i = f2D(i, h - 1, w, h);
		int stored_value = layer[final_i];
		for (int j = 0; j < h - 1; j++)
		{
			int pos = f2D(i, j, w, h);
			IntPair next = move_pos_wrapped_2d_down(math_intpair_create(i, j), w, h);
			int next_1d = f2D(next.x, next.y, w, h);
			layer[next_1d] = (layer[pos] * is_moving[pos]) + (layer[next_1d] * (1 - is_moving[pos]));
		}

		IntPair next_final_2d = move_pos_wrapped_2d_down(math_intpair_create(i, h - 1), w, h);
		int next_final_1d = f2D(next_final_2d.x, next_final_2d.y, w, h);
		layer[next_final_1d] = (stored_value * is_moving[final_i]) + (layer[next_final_1d] * (1 - is_moving[final_i]));
	}
}

//******************************DIRECTION UTILITY***************************************/
IntPair direction_to_intpair(Direction action)
{
	if (action == U)
	{
		return math_intpair_create(0, 1);
	}
	else if (action == R)
	{
		return math_intpair_create(1, 0);
	}
	else if (action == D)
	{
		return math_intpair_create(0, -1);
	}
	else if (action == L)
	{
		return math_intpair_create(-1, 0);
	}
	return math_intpair_create(0, 0);
}
Direction action_to_direction(Action action)
{
	if (action == Action::A_UP)
		return Direction::U;
	if (action == Action::A_RIGHT)
		return Direction::R;
	if (action == Action::A_DOWN)
		return Direction::D;
	if (action == Action::A_LEFT)
		return Direction::L;
	return Direction::NO_DIRECTION;
}
//**********************timemachine utilty***********************************/
void gamestate_timemachine_reset(GamestateTimeMachine* timeMachine, Memory* scope_memory)
{
	int next_pos = timeMachine->num_gamestates_stored;
	GameState* next = &timeMachine->state_array[next_pos];
	GameState* old = &timeMachine->state_array[0];
	gamestate_clone_to_unitialized(old, next, scope_memory);
	timeMachine->num_gamestates_stored++;
}
static void apply_merge_moves(PieceData* piece_data, bool* is_moving, int* layer, Direction d, int w, int h, GameStateAnimation* animation)
{
	bool done = false;
	while (!done)
	{
		done = true;
		int length = w * h;
		int z = 0;
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++, z++)
			{
				if (is_moving[z])
				{
					IntPair next = move_pos_wrapped_2d(math_intpair_create(i, j), d, w, h);
					int next_1d = f2D(next.x, next.y, w, h);
					bool we_are_merge = piece_data[z].powers[CP_MERGE] || piece_data[next_1d].powers[CP_MERGE];
					bool both_crates = layer[next_1d] == P_CRATE && layer[z] == P_CRATE;
					bool next_is_blocked = (layer[next_1d] != LN_FLOOR) && !is_moving[next_1d];
					if (next_is_blocked)
					{
						if (both_crates && we_are_merge)
						{
							//get the next crate's piece_data, and our piece_data, and perform a merge.
							piece_data[next_1d].powers[CP_PUSH] = piece_data[next_1d].powers[CP_PUSH] || piece_data[z].powers[CP_PUSH];
							piece_data[next_1d].powers[CP_PULL] = piece_data[next_1d].powers[CP_PULL] || piece_data[z].powers[CP_PULL];
							piece_data[next_1d].powers[CP_MERGE] = piece_data[next_1d].powers[CP_MERGE] || piece_data[z].powers[CP_MERGE];
							piece_data[next_1d].powers[CP_PARALLEL] = piece_data[next_1d].powers[CP_PARALLEL] || piece_data[z].powers[CP_PARALLEL];
							layer[z] = 0;
							for(int q = 0; q < CP_COUNT;q++)
								piece_data[z].powers[q] = false;
							is_moving[z] = false;

							//put that informataion into the animation.
							animation->symbol_start.end_powers[z] = piece_data[next_1d];
							animation->symbol_start.end_powers[next_1d] = piece_data[next_1d];
						}
					}
				}
			}
	}
}
static void cancel_moves_by_rigid(RigidData* rigids, bool* is_moving, int rigid_to_cancel, int len)
{
	for (int k = 0; k < len; k++)
		if (rigids->grid[k] == rigid_to_cancel)
			is_moving[k] = false;
	for (int z = 0; z < rigids->num_links; z++)
		if (rigids->links[z].x == rigid_to_cancel)
			cancel_moves_by_rigid(rigids, is_moving, rigids->links[z].y, len);
}
static void cancel_blocked_nonmerge_moves(RigidData* rigids, bool* is_moving, int* layer, PieceData* piece_data, Direction d, int w, int h)
{
	bool loop = false;
	bool done = false;
	while (!done)
	{
		done = true;
		int length = w * h;
		int z = 0;
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++, z++)
			{
				if (is_moving[z])
				{
					IntPair next = move_pos_wrapped_2d(math_intpair_create(i, j), d, w, h);
					int next_1d = f2D(next.x, next.y, w, h);
					bool we_are_merge = piece_data[z].powers[CP_MERGE] || piece_data[next_1d].powers[CP_MERGE];
					bool both_crates = layer[next_1d] == P_CRATE && layer[z] == P_CRATE;
					bool next_is_blocked = (layer[next_1d] != LN_FLOOR) && !is_moving[next_1d];
					if (next_is_blocked)
					{
						if (both_crates && we_are_merge)
						{

						}
						else
						{
							is_moving[z] = false;
							done = false;
							if (rigids->grid[z] != 0)
							{
								cancel_moves_by_rigid(rigids,is_moving,rigids->grid[z],w*h);
								loop = true;
							}
						}
					}
				}
			}
	}
	if (loop)
		cancel_blocked_nonmerge_moves(rigids, is_moving, layer, piece_data, d, w, h);
}
static void cancel_blocked_moves(bool* is_moving, int* layer, Direction d, int w, int h)
{
	bool done = false;
	while (!done)
	{
		done = true;
		int length = w * h;
		int z = 0;
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++, z++)
			{
				if (is_moving[z])
				{
					IntPair next = move_pos_wrapped_2d(math_intpair_create(i, j), d, w, h);
					int next_1d = f2D(next.x, next.y, w, h);
					bool next_is_blocked = (layer[next_1d] != LN_FLOOR) && !is_moving[next_1d];
					if (next_is_blocked)
					{
						is_moving[z] = false;
						done = false;
					}
				}
			}
	}

}
GamestateTimeMachine* gamestate_timemachine_create(GameState* start_state, Memory* memory, int max_num_gamestates)
{
	GamestateTimeMachine* result = (GamestateTimeMachine*) memory_alloc(memory, sizeof(GamestateTimeMachine));
	result->start_state = gamestate_clone(start_state, memory);
	result->state_array = (GameState*)memory_alloc(memory, sizeof(GameState) * max_num_gamestates);
	//TODO: Get this function actually working correctly. Do eet.
	gamestate_clone_to_unitialized(result->start_state, &(result->state_array[0]), memory);
	//gamestate_clone_to(start_state, result->state_array);
	result->num_gamestates_stored = 1;
	result->max_gamestates_storable = max_num_gamestates;
	return result;
}
void* gamestate_timemachine_undo(GamestateTimeMachine* timeMachine)
{
	//returns a pointer to the memory we cleared.
	if (timeMachine->num_gamestates_stored > 1)
	{
		timeMachine->num_gamestates_stored--;
		//TODO: 
		return &timeMachine->state_array[timeMachine->num_gamestates_stored];
	}
	return NULL;
}
GameState* gamestate_timemachine_get_latest_gamestate(GamestateTimeMachine* timeMachine)
{
	return &timeMachine->state_array[timeMachine->num_gamestates_stored - 1];
}

//*****************************************GAMESTATE WRITE************************//
GameState* gamestate_create(Memory* memory, int w, int h)
{
	int size = sizeof(GameState);
	GameState* result = (GameState*) memory_alloc(memory,size);
	gamestate_allocate_layers(result, memory, w, h);
	return result;
}
GameState* gamestate_clone(GameState* state, Memory* memory)
{
	GameState* clone_to = gamestate_create(memory, state->w, state->h);
	gamestate_clone_to(state, clone_to);
	return clone_to;
}
GameState* gamestate_merge_with_allocate(GameState* first, GameState* second, IntPair combined_size, Memory* output_memory, IntPair left_merge_offset, IntPair right_merge_offset)
{
	GameState* output_state = gamestate_create(output_memory, combined_size.x, combined_size.y);
	gamestate_merge(first, second, output_state, left_merge_offset, right_merge_offset);
	return output_state;
}
GameState* gamestate_resize_with_allocate(GameState* input_state, Memory* output_memory, int output_w, int output_h, IntPair displacement_from_input_to_output)
{
	GameState* output_state = gamestate_create(output_memory, output_w, output_h);
	gamestate_resize(input_state, output_state, displacement_from_input_to_output);
	return output_state;
}
GameActionJournal* gamestate_timemachine_take_action(GamestateTimeMachine* timeMachine, Direction action, Memory* scope_memory, Memory* temp_memory)
{
	int next_pos = timeMachine->num_gamestates_stored;
	int old_pos = next_pos - 1;
	GameState* old = &timeMachine->state_array[old_pos];
	GameState* next = &timeMachine->state_array[next_pos];
	gamestate_clone_to_unitialized(old, next, scope_memory);
	GameActionJournal* result = gamestate_action(next, action, temp_memory);
	timeMachine->num_gamestates_stored++;
	return result;
}
void gamestate_merge(GameState* left, GameState* right, GameState* output, IntPair left_merge_offset, IntPair right_merge_offset)
{
	int w_f = left->w;
	int h_f = left->h;
	int w_s = right->w;
	int h_s = right->h;
	int w_out = output->w;
	int h_out = output->h;
	
	//merge floor
	{
		for (int i = 0; i < w_f; i++)
			for (int j = 0; j < h_f; j++)
			{
				int index_in = f2D(i, j, w_f, h_f);
				int index_out = f2D(i + left_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->floor[index_out] = left->floor[index_in];
			}
		for (int i = 0; i < w_s; i++)
			for (int j = 0; j < h_s; j++)
			{
				int index_in = f2D(i, j, w_s, h_s);
				int index_out = f2D(i + right_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->floor[index_out] = left->floor[index_in];
			}
	}
	//merge piece
	{
		for (int i = 0; i < w_f; i++)
			for (int j = 0; j < h_f; j++)
			{
				int index_in = f2D(i, j, w_f, h_f);
				int index_out = f2D(i + left_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->piece[index_out] = left->piece[index_in];
			}
		for (int i = 0; i < w_s; i++)
			for (int j = 0; j < h_s; j++)
			{
				int index_in = f2D(i, j, w_s, h_s);
				int index_out = f2D(i + right_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->piece[index_out] = left->piece[index_in];
			}
	}
	//merge floor_data
	{
		for (int i = 0; i < w_f; i++)
			for (int j = 0; j < h_f; j++)
			{
				int index_in = f2D(i, j, w_f, h_f);
				int index_out = f2D(i + left_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->floor_data[index_out] = left->floor_data[index_in];
			}
		for (int i = 0; i < w_s; i++)
			for (int j = 0; j < h_s; j++)
			{
				int index_in = f2D(i, j, w_s, h_s);
				int index_out = f2D(i + right_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->floor_data[index_out] = left->floor_data[index_in];
			}
	}
	//merge piece_data
	{
		for (int i = 0; i < w_f; i++)
			for (int j = 0; j < h_f; j++)
			{
				int index_in = f2D(i, j, w_f, h_f);
				int index_out = f2D(i + left_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->piece_data[index_out] = left->piece_data[index_in];
			}
		for (int i = 0; i < w_s; i++)
			for (int j = 0; j < h_s; j++)
			{
				int index_in = f2D(i, j, w_s, h_s);
				int index_out = f2D(i + right_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->piece_data[index_out] = left->piece_data[index_in];
			}
	}

}
GameState* gamestate_add_row(GameState* input, Memory* scope_memory, int target_x, int target_y)
{
	int w_old = input->w;
	int h_old = input->h;
	int w_next = w_old + 1;
	int h_next = h_old;
	GameState* output = gamestate_create(scope_memory, w_next, h_next);
	for (int i = 0; i < w_old; i++)
		for (int j = 0; j < h_old; j++)
		{
			int out_x = i + (i >= target_x);
			int out_y = j;
			int in_index = f2D(i, j, w_old, h_old);
			int out_index = f2D(out_x, out_y, w_next, h_next);
			{
				output->floor[out_index] = input->floor[in_index];
				output->piece[out_index] = input->piece[in_index];
				output->floor_data[out_index] = input->floor_data[in_index];
				output->piece_data[out_index] = input->piece_data[in_index];
	
			}
		}
	return output;
}
GameState* gamestate_add_column(GameState* input, Memory* scope_memory, int target_x, int target_y)
{
	int w_old = input->w;
	int h_old = input->h;
	int w_next = w_old;
	int h_next = h_old + 1;
	GameState* output = gamestate_create(scope_memory, w_next, h_next);
	for (int i = 0; i < w_old; i++)
		for (int j = 0; j < h_old; j++)
		{
			int out_x = i;
			int out_y = j + (j >= target_y);
			int in_index = f2D(i, j, w_old, h_old);
			int out_index = f2D(out_x, out_y, w_next, h_next);
			{
				output->floor[out_index] = input->floor[in_index];
				output->piece[out_index] = input->piece[in_index];
				output->floor_data[out_index] = input->floor_data[in_index];
				output->piece_data[out_index] = input->piece_data[in_index];
			}
		}
	return output;
}
GameState* gamestate_surround_with_walls(GameState* input, Memory* scope_memory)
{
	int old_w = input->w;
	int old_h = input->h;
	int next_w = old_w + 2;
	int next_h = old_h + 2;
	GameState* result = gamestate_create(scope_memory, next_w, next_h);
	for (int i = 0; i < next_w; i++)
		for (int j = 0; j < next_h; j++)
			if (i == 0 || j == 0 || i == next_w - 1 || j == next_h - 1)
			{
				int z = f2D(i, j, next_w,next_h);
				result->piece[z] = P_WALL;
			}

		for (int i = 0; i < old_w; i++)
			for (int j = 0; j < old_h; j++)
			{
				int in_index = f2D(i, j, old_w, old_h);
				int out_index = f2D(i + 1, j + 1, next_w, next_h);
				result->floor[out_index] = input->floor[in_index];
				result->piece[out_index] = input->piece[in_index];
				result->floor_data[out_index] = input->floor_data[in_index];
				result->piece_data[out_index] = input->piece_data[in_index];

			}
	return result;
}
//*******************************GAMESTATE ACTION INTERNALS************************/
void gamestate_allocate_layers(GameState* result, Memory* memory, int w, int h)
{
	result->w = w;
	result->h = h;
	result->piece = (int*)memory_alloc(memory, sizeof(int) * w * h);
	result->floor = (int*)memory_alloc(memory, sizeof(int) * w * h);
	result->piece_data = (PieceData*)memory_alloc(memory, sizeof(PieceData) * w * h);
	result->floor_data = (FloorData*)memory_alloc(memory, sizeof(FloorData) * w * h);
	for (int i = 0; i < w * h; i++)
	{
		Floor f = (Floor)0;
		result->floor[i] = f;
	}
	for (int i = 0; i < w * h; i++)
	{
		Piece p = (Piece)0;
		result->piece[i] = p;
	}
	for (int i = 0; i < w * h; i++)
	{
		result->floor_data[i] = gamestate_floordata_make();
	}
	for (int i = 0; i < w * h; i++)
	{
		result->piece_data[i] = gamestate_piecedata_make();
	}

}
bool gamestate_is_in_win_condition(GameState* state)
{
	int len = state->w * state->h;
	for (int i = 0; i < len; i++)
	{
		if (state->floor[i] == F_EXIT || state->floor[i] == F_START || state->floor[i] == F_STAIRCASE_LEVELSTART)
			if(!is_player(state->piece[i]))
			{
				return false;
			}
	}
	for (int i = 0; i < len; i++)
	{
		if (state->floor[i] == F_TARGET && state->piece[i] != P_CRATE) 
			return false;
	}
	return true;
}

int maybe_find_exit_or_return_neg1(GameState* state)
{
	int len = state->w * state->h;
	for (int i = 0; i < len; i++)
	{
		if (state->floor[i] == Floor::F_EXIT)
		{
			return i;
			break;
		}
	}
	for (int i = 0; i < len; i++)
	{
		if (state->floor[i] == Floor::F_START)
		{
			return i;
			break;
		}
	}
	for (int i = 0; i < len; i++)
	{
		if (state->floor[i] == Floor::F_STAIRCASE_LEVELSTART)
		{
			return i;
			break;
		}
	}
	return -1;
}

int gamestate_maybe_find_player(GameState* state)
{
	for (int i = 0; i < state->w * state->h; i++)
		if (state->piece[i] == Piece::P_PLAYER)
			return i;
	return -1;
}
bool is_level_start(int val)
{
	return val == F_START || val == F_STAIRCASE_LEVELSTART;
}
bool is_staircase(int val)
{
	return val == F_STAIRCASE || val == F_STAIRCASE_LEVELSTART || val == F_STAIRCASE_SOLVED;
}
bool is_player(int val)
{
	return val == P_PLAYER;
}
bool is_crate(int val)
{
	return val == P_CRATE;
}
bool is_normal_crate(PieceData val)
{
	return val.powers[CP_PUSH];
}
bool is_pull_crate(PieceData val)
{
	return val.powers[CP_PULL];
}
void gamestate_resize(GameState* input_state, GameState* output_state, IntPair displacement_from_input_to_output)
{
	int w_in = input_state->w;
	int h_in = input_state->h;
	int w_out = output_state->w;
	int h_out = output_state->h;
	for(int i = 0; i < w_in;i++)
		for (int j = 0; j < h_in; j++)
		{
			int out_x = i + displacement_from_input_to_output.x;
			int out_y = j + displacement_from_input_to_output.y;
			if (out_x >= 0 && out_x < w_out && out_y >= 0 && out_y < h_out)
			{
				int in_index = f2D(i, j, w_in, h_in);
				int out_index = f2D(out_x, out_y, w_out, h_out);
				{
					output_state->floor[out_index] = input_state->floor[in_index];
					output_state->piece[out_index] = input_state->piece[in_index];
					output_state->floor_data[out_index] = input_state->floor_data[in_index];
					output_state->piece_data[out_index] = input_state->piece_data[in_index];
				}
			}

		}
}
void  gamestate_clone_to(GameState* input_state, GameState* output_state)
{
	int w = input_state->w;
	int h = input_state->h;
	if (output_state->w != w || output_state->h != h)
	{
		crash_err("You fool! You tried to use gamestate_clone_to on an input_state that didn't have the correct amount of memory. This would be a memory leak. It's now a crash. Enjoy.");
	}
	output_state->w = w;
	output_state->h = h;
	for(int i = 0; i < w;i++)
		for (int j = 0; j < h; j++)
		{
			{
				int index = f2D(i, j, w, h);
				output_state->floor[index] = input_state->floor[index];
				output_state->piece[index] = input_state->piece[index];
				output_state->floor_data[index] = input_state->floor_data[index];
				output_state->piece_data[index] = input_state->piece_data[index];
				
			}
		}
}
void gamestate_clone_to_unitialized(GameState* input_state, GameState* output_state, Memory* memory)
{
	int w = input_state->w;
	int h = input_state->h;
	gamestate_allocate_layers(output_state, memory, w, h);
	gamestate_clone_to(input_state, output_state);
}

//returns whether or not to reject this brush action as invalid because it made no change.
bool gamestate_apply_brush(GameState* state, GamestateBrush brush, int x, int y)
{
	bool accept = false;
	if (brush.applyFloor)
	{
		int w = state->w;
		int h = state->h;
		int target = f2D(x, y, w,h);
		if (state->floor[target] != brush.floor)
		{
			state->floor[target] = brush.floor;
			accept = true;
		}

	}
	if (brush.applyPiece)
	{
		int w = state->w;
		int h = state->h;
		int target = f2D(x, y, w, h);
		if (state->piece[target] != brush.piece)
		{
			state->piece[target] = brush.piece;
			accept = true;
		}
	}
	if (brush.applyCratePower)
	{
		int w = state->w;
		int h = state->h;
		int target = f2D(x, y, w, h);
		for (int i = 0; i < CP_COUNT; i++)
			state->piece_data[target].powers[i] = brush.piece_data.powers[i];
	}
	if (brush.applyFloorPower)
	{
		int w = state->w;
		int h = state->h;
		int target = f2D(x, y, w,h);
		{
			state->floor_data[target] = brush.floor_data;
		}
	}
	return accept;
}
GamestateBrush gamestate_brush_create(bool applyFloor, Floor floor, bool applyPiece, Piece piece, bool applyPieceData, PieceData pieceData,bool applyFloorData, FloorData floorData)
{
	GamestateBrush result;
	result.applyFloor = applyFloor;
	result.floor = floor;
	result.applyPiece = applyPiece;
	result.piece = piece;
	result.applyCratePower = applyPieceData;
	result.piece_data = pieceData;
	result.applyFloorPower = applyFloorData;
	result.floor_data = floorData;
	return result;
}

/*************************PIECE/FLOOR DATA*******************************/
/*********************************************************************/
textureAssets::SYMBOLS piecedata_to_symbol(CratePower val)
{
	if (val == CratePower::CP_PUSH)
		return textureAssets::SYMBOLS::Circle;
	if (val == CratePower::CP_PULL)
		return textureAssets::SYMBOLS::Square;
	if (val == CratePower::CP_MERGE)
		return textureAssets::SYMBOLS::Plus;
	if (val == CratePower::CP_PARALLEL)
		return textureAssets::SYMBOLS::Triangle;
	crash_err("Tried to draw a crate_power that is not drawable");
	return textureAssets::SYMBOLS::HalfCircle;
}

PieceData gamestate_piecedata_make()
{
	PieceData result;
	for (int i = 0; i < CP_COUNT; i++)
		result.powers[i] = false;
	return result;
}
PieceData gamestate_piecedata_make(CratePower power)
{
	PieceData result;
	for (int i = 0; i < CP_COUNT; i++)
		result.powers[i] = false;
	result.powers[power] = true;
	return result;

}
PieceData gamestate_piecedata_make(CratePower power,CratePower power2)
{
	PieceData result;
	for (int i = 0; i < CP_COUNT; i++)
		result.powers[i] = false;
	result.powers[power] = true;
	result.powers[power2] = true;
	return result;

}
PieceData gamestate_piecedata_make(CratePower power,CratePower power2, CratePower power3)
{
	PieceData result;
	for (int i = 0; i < CP_COUNT; i++)
		result.powers[i] = false;
	result.powers[power] = true;
	result.powers[power2] = true;
	result.powers[power3] = true;
	return result;

}
FloorData gamestate_floordata_make()
{
	FloorData result;
	result.level_index = -1;
	for (int i = 0; i < MAX_REQUIREMENTS; i++)
		result.requirements[i] = -1;
	result.teleporter_target_square = math_intpair_create(-1, -1);
	result.require_all_requirements_met_not_just_one = true;
	return result;
}

/******************************GAMESTATE READ************************/
/********************************************************************/
int gamestate_calc_clicked_state(GameState** states, IntPair* states_positions, int num_states, float mouse_game_pos_x,float mouse_game_pos_y)
{
	for (int i = 0; i < num_states; i++)
	{
		GameState* currentState = states[i];
		float left = (float) states_positions[i].x;
		float right = (float) states_positions[i].x + currentState->w;
		float down = (float) states_positions[i].y;
		float up = (float) states_positions[i].y + currentState->h;
		bool clickedFloor = math_click_is_inside_AABB(left, down, right, up, mouse_game_pos_x, mouse_game_pos_y);
		if(clickedFloor)
			return i;
	}
	return -1;
}
WorldPosition gamestate_calc_clicked_world_position(GameState** states, IntPair* states_positions, int num_states, float mouse_game_pos_x, float mouse_game_pos_y)
{
	glm::vec2 mouseGamePos = glm::vec2(mouse_game_pos_x, mouse_game_pos_y);
	int index_clicked = gamestate_calc_clicked_state(states,states_positions,num_states, mouse_game_pos_x, mouse_game_pos_y);
	if (index_clicked == -1)
		return world_make_world_position(-1, math_intpair_create(-1, -1), -1);
	GameState* state = states[index_clicked];
	IntPair state_start = states_positions[index_clicked];
	IntPair grid_clicked;
	{
		float left = (float) state_start.x;
		float right = (float) (state_start.x + state->w);
		float down = (float) state_start.y;
		float up = (float) (state_start.y + state->h);
		//calculate what floor cell we actually clicked.
		float percentageX = percent_between_two_points(mouseGamePos.x, left, right);
		float percentageY = percent_between_two_points(mouseGamePos.y, down, up);
		int x_floor_cell_clicked = (int)(percentageX * state->w);
		int y_floor_cell_clicked = (int)(percentageY * state->h);

		grid_clicked = math_intpair_create(x_floor_cell_clicked, y_floor_cell_clicked);
	}
	int grid_clicked_1d = f2D(grid_clicked.x, grid_clicked.y, state->w, state->h);
	WorldPosition result = world_make_world_position(index_clicked, grid_clicked, grid_clicked_1d);
	return result;
}

WorldPosition gamestate_get_position_linked_by_teleporter(GameState** gamestates, int len, WorldPosition first_staircase)
{
	//1: get the staircase linked by the first staircase.
	WorldPosition second_staircase;
	{
		GameState* second_level = gamestates[first_staircase.level_index];
		FloorData floor_data = second_level->floor_data[first_staircase.level_position_1d];
		second_staircase.level_index = floor_data.level_index;
		second_staircase.level_position = floor_data.teleporter_target_square;
		int link_w = gamestates[second_staircase.level_index]->w;
		int link_h = gamestates[second_staircase.level_index]->h;
		second_staircase.level_position_1d = f2D(second_staircase.level_position.x, second_staircase.level_position.y, link_w, link_h);
	}
	return second_staircase;
}
WorldPosition gamestate_get_position_linked_by_teleporter_and_check_backlink(GameState** gamestates, int len, WorldPosition first_staircase) /*returns the staircase link, if it exists.*/
{
	WorldPosition second_staircase = gamestate_get_position_linked_by_teleporter(gamestates, len, first_staircase);
	WorldPosition first_staircase_check = gamestate_get_position_linked_by_teleporter(gamestates, len, second_staircase);
	//3: error check. 
	{
		if (first_staircase_check.level_index != first_staircase.level_index ||
			first_staircase_check.level_position.x != first_staircase.level_position.x ||
			first_staircase_check.level_position.y != first_staircase.level_position.y ||
			first_staircase_check.level_position_1d != first_staircase.level_position_1d)
				crash_err("tried to get a staircase link, but its broken.");
	}
	return second_staircase;
}
void gamestate_print_staircase_tele_value(GameState** gamestate, int len)
{
	for (int z = 0; z < len; z++)
	{
		int w = gamestate[z]->w;
		int h = gamestate[z]->h;
		for (int i = 0; i < w * h; i++)
		{
			if (is_staircase(gamestate[z]->floor[i]))
			{
				std::cout << "i" << i << ",tele_link" << gamestate[z]->floor_data[i].level_index << std::endl;
			}
		}
	}
}
int** gamestate_get_layers(GameState* gamestate, int* num_layers_found, Memory* temp_memory)
{
	int** result = (int**)memory_alloc(temp_memory, sizeof(int*) * 2);
	result[0] = gamestate->floor;
	result[1] = gamestate->piece;
	return result;
}
int* gamestate_get_layer(GameState* gamestate, int layer_num)
{
	if (layer_num == 0)
	return gamestate->floor;
	if (layer_num == 1)
		return gamestate->piece;
	crash_err("uh ohh, we tried to get a layer that doesn't exist!");
	return gamestate->floor;
}
AABB* gamestate_create_colliders(Memory* memory, GameState** states, IntPair* locations, int length)
{
	AABB* result = (AABB*)memory_alloc(memory, sizeof(AABB) * (length - 1));
	for (int i = 0; i < length; i++)
	{
		result[i].w = (float) states[i]->w;
		result[i].h = (float) states[i]->h;
	}
	for (int i = 0; i < length; i++)
	{
		result[i].x = (float) locations[i].x;
		result[i].y = (float) locations[i].y;
	}
	return result;
}
AABB* gamestate_create_colliders(Memory* memory, GameState** states, IntPair* locations, int length, int skip_index)
{
	AABB* result = (AABB*) memory_alloc(memory, sizeof(AABB) * (length - 1));
	for (int i = 0; i < skip_index; i++)
	{
		result[i].w = (float) states[i]->w;
		result[i].h = (float) states[i]->h;
	}
	for (int i = skip_index + 1; i < length; i++)
	{
		result[i - 1].w = (float) states[i]->w;
		result[i - 1].h = (float) states[i]->h;
	}
	for (int i = 0; i < skip_index; i++)
	{
		result[i].x = (float) locations[i].x;
		result[i].y = (float) locations[i].y;
	}
	for (int i = skip_index + 1; i < length; i++)
	{
		result[i - 1].x = (float) locations[i].x;
		result[i - 1].y = (float) locations[i].y;
	}
	return result;
}
void symbollocalanimation_alloc_internal(GameState* state, SymbolAnimation* animation,int w, int h)
{
		int z = 0;
		for (int i = 0; i < w; i++)
			for(int j = 0; j < h;j++,z++)
		{
				animation->start_powers[z] = state->piece_data[z];
				animation->end_powers[z] = state->piece_data[z];
				for (int p = 0; p < CP_COUNT; p++)
					animation->flash_powers[z].powers[p] = false;;
				animation->global_position[z] = math_intpair_create(i, j);
				animation->global_move[z] = Direction::NO_DIRECTION;
		}
}
	
SymbolAnimation* symbollocalanimation_create_internal(int num_elements, Memory* temp_memory)
{
	SymbolAnimation* result = mem_alloc<SymbolAnimation>(temp_memory, 1);
	result->len = num_elements;
	result->global_position = mem_alloc<IntPair>(temp_memory, num_elements);
	result->global_move = mem_alloc<Direction>(temp_memory,num_elements);
	result->start_powers = mem_alloc<PieceData>(temp_memory,num_elements); //true if should be drawn.
	result->end_powers = mem_alloc<PieceData>(temp_memory,num_elements);
	result->flash_powers = mem_alloc<PieceData>(temp_memory,num_elements);
	return result;
}

void symbolglobalanimation_alloc_internal(SymbolGlobalAnimation* to_alloc, Memory* temp_memory,int num_elements)
{
	to_alloc->len = num_elements;
	to_alloc->img_value = mem_alloc<int>(temp_memory, num_elements);
	to_alloc->to_move = mem_alloc<Direction>(temp_memory, num_elements);
	to_alloc->pos = mem_alloc<IntPair>(temp_memory, num_elements);
}
void copy_piece_animation_to_symbol_animation(PieceData* crates, int len_pieces, int len_symbols, MaskedMovementAnimation* pieces, MaskedMovementAnimation* symbol)
{
	int current_len_symbols = 0;
	for (int i = 0; i < len_pieces; i++)
	{
		for(int j = 0; j < CP_COUNT;j++)
			if (crates[i].powers[j])
			{
				symbol->pos[current_len_symbols] = pieces->pos[i];
				symbol->img_value[current_len_symbols] = j;
				symbol->to_move[current_len_symbols]= pieces->to_move[i];
				current_len_symbols++;
			}
	}
}
void masked_movement_animation_clone_internal(int len, MaskedMovementAnimation* copy, MaskedMovementAnimation* original)
{
	for (int i = 0; i < len; i++)
	{
		copy->img_value[i] = original->img_value[i];
	}
	for (int i = 0; i < len; i++)
	{
		copy->pos[i] = original->pos[i];
	}
	for (int i = 0; i < len; i++)
	{
		copy->to_move[i] = original->to_move[i];
	}
}
MaskedMovementAnimation* gamestate_animationmoveinfo_create_internal(int num_elements, Memory* temp_memory)
{
	MaskedMovementAnimation* result = (MaskedMovementAnimation*)memory_alloc(temp_memory, sizeof(MaskedMovementAnimation));
	result->pos = (IntPair*)memory_alloc(temp_memory, sizeof(IntPair) * num_elements);
	result->to_move = (Direction*)memory_alloc(temp_memory, sizeof(Direction) * num_elements);
	result->img_value = (int*)memory_alloc(temp_memory, sizeof(int) * num_elements);
	for (int i = 0; i < num_elements; i++)
	{
		result->pos[i] = math_intpair_create(0, 0);
		result->to_move[i] = Direction::NO_DIRECTION;
		result->img_value[i] = 0;
	}
	return result;
}
void animationmoveinfo_set_default_positions(MaskedMovementAnimation* info, GameState* state)
{
	int w = state->w;
	int h = state->h;
	int z = 0;
	for(int i = 0; i < w;i++)
		for (int j = 0; j < h; j++, z++)
		{
			info->pos[z] = math_intpair_create(i, j);
		}
}
void animationmoveinfo_copy_from_gamestate_internal(MaskedMovementAnimation* info, GameState* state)
{
	int num_to_draw = state->w * state->h;
	for (int i = 0; i < num_to_draw; i++)
	{
		int next_val = state->piece[i];
		info->img_value[i] = next_val;
	}

}
void animationmoveinfo_apply_movements(GameState* state, MaskedMovementAnimation* start,MaskedMovementAnimation* end, bool* moves, Direction movement, int num_to_draw)
{
	int z = 0;
	for(int i = 0; i < state->w;i++)
		for(int j = 0;j < state->h;j++,z++)
		if (moves[z])
		{
			start->to_move[z] = movement;
			end->to_move[z] = movement;
			IntPair next_goal = move_pos_wrapped_2d(math_intpair_create(i,j),movement, state->w, state->h);
			end->pos[z] = next_goal;
		}
}

glm::vec2 piecedata_calculate_scale(PieceData piece)
{
	int total = 0;
	for (int i = 0; i < CP_COUNT; i++)
		total += piece.powers[i];
	int width = (int)ceil(sqrt(total));
	glm::vec2 scale;
	scale.x = 1.0f / (float)width;
	scale.y = 1.0f / (float)width;
	return scale;
}

void piecedata_calculate_local_positions(PieceData piece, glm::vec2* scale, glm::vec2* out_positions)
{
	int total = 0;
	for (int i = 0; i < CP_COUNT; i++)
		total += piece.powers[i];
	int width = (int)ceil(sqrt(total));

	int num_drawn = 0;
	for (int j = 0; j < CP_COUNT; j++)
	{
		if (piece.powers[j])
		{
			out_positions[num_drawn].x = float(num_drawn % width);
			out_positions[num_drawn].y = float(num_drawn / width);
			num_drawn++;
		}
	}
}

/****************************GAME ACTION + MISC**********************/
/********************************************************************/
GameActionJournal* gamestate_action(GameState* state, Direction action, Memory* temp_memory)
{
	GameActionJournal* journal = (GameActionJournal*)memory_alloc(temp_memory, sizeof(GameActionJournal));
	journal->old_state = gamestate_clone(state, temp_memory);
	const int w = state->w;
	const int h = state->h;
	const int num_elements = w * h;
	int* pieces = state->piece;

	//build the gamestate animation, then set the results to default values.
	GameStateAnimation* animation;
	{
		animation = (GameStateAnimation*) memory_alloc(temp_memory, sizeof(GameStateAnimation));
		animation->starts = *gamestate_animationmoveinfo_create_internal(w * h, temp_memory);
		animation->ends = *gamestate_animationmoveinfo_create_internal(w * h, temp_memory);
		animation->num_to_draw = w * h;
		animationmoveinfo_copy_from_gamestate_internal(&animation->starts, state);
		animationmoveinfo_set_default_positions(&animation->starts, state);
		animationmoveinfo_set_default_positions(&animation->ends, state);
	}
	//find the player.
	int player_1d = 0;
	IntPair player_2d = math_intpair_create(0,0);
	{

		for (int i = 0; i < num_elements; i++)
		{
			if (is_player(pieces[i]))
			{
				//we found a player that isn't having their move stopped, continue the algorithm.
				player_1d = i;
				player_2d = t2D(i, state->w, state->h);
				break;
			}
		}
	}

	//build an array of moving for the "pieces" layer.
	bool* square_moving = (bool*)memory_alloc(temp_memory, sizeof(bool) * num_elements);
	{
		memset(square_moving, false, sizeof(char) * num_elements);
		square_moving[player_1d] = true;
	}

	//build the starting rigid data.
	RigidData rigid_data;
	{
		rigid_data.grid = (int*)memory_alloc(temp_memory, sizeof(int) * num_elements);
		for (int i = 0; i < num_elements; i++)
			rigid_data.grid[i]= 0;
		rigid_data.links = (IntPair*) memory_alloc(temp_memory, sizeof(IntPair) * num_elements);
		rigid_data.num_links = 0;
		rigid_data.num_rigids = 1;
	}
	//for each moving player, if their is a crate in their movement direction, make that move too! also cancel grill stuff.
	{
		IntPair next_square = move_pos_wrapped_2d(player_2d, action, w, h);
		IntPair back_square = move_pos_wrapped_2d(player_2d, direction_reverse(action), w, h);
		IntPair clockwise_square = move_pos_wrapped_2d(player_2d, direction_rotate_clockwise(action), w, h);
		IntPair anticlockwise_square = move_pos_wrapped_2d(player_2d, direction_rotate_anti(action), w, h);
		int next_square_1d = f2D(next_square.x, next_square.y, w, h);
		int back_square_1d = f2D(back_square.x, back_square.y, w, h);
		int clockwise_square_1d = f2D(clockwise_square.x, clockwise_square.y, w, h);
		int anticlockwise_square_1d = f2D(anticlockwise_square.x, anticlockwise_square.y, w, h);

		int next_piece = pieces[next_square_1d];
		int back_piece = pieces[back_square_1d];
		int clockwise_piece = pieces[clockwise_square_1d];
		int anticlockwise_piece = pieces[anticlockwise_square_1d];

		//setup player rigidnumber.
		int player_rigid_number = rigid_data.num_rigids;
		rigid_data.num_rigids++;
		rigid_data.grid[player_1d] = player_rigid_number;
		if (next_piece == P_CRATE && state->piece_data[next_square_1d].powers[CP_PUSH])
		{
			square_moving[next_square_1d] = true;
			rigid_data.grid[next_square_1d] = rigid_data.num_rigids;
			rigid_data.num_rigids++;
			rigid_data.links[rigid_data.num_links] = math_intpair_create(player_rigid_number, rigid_data.grid[next_square_1d]);
			rigid_data.num_links++;
		}
		if (back_piece == P_CRATE && state->piece_data[back_square_1d].powers[CP_PULL])
		{
			square_moving[back_square_1d] = true;
			rigid_data.grid[back_square_1d] = rigid_data.num_rigids;
			rigid_data.num_rigids++;
			rigid_data.links[rigid_data.num_links] = math_intpair_create(player_rigid_number, rigid_data.grid[back_square_1d]);
			rigid_data.num_links++;
		}
		if (clockwise_piece == P_CRATE && state->piece_data[clockwise_square_1d].powers[CP_PARALLEL])
		{
			square_moving[clockwise_square_1d] = true;
			rigid_data.grid[clockwise_square_1d] = rigid_data.num_rigids;
			rigid_data.num_rigids++;
			rigid_data.links[rigid_data.num_links] = math_intpair_create(player_rigid_number, rigid_data.grid[clockwise_square_1d]);
			rigid_data.num_links++;
		}
		if (anticlockwise_piece == P_CRATE && state->piece_data[anticlockwise_square_1d].powers[CP_PARALLEL])
		{
			square_moving[anticlockwise_square_1d] = true;
			rigid_data.grid[anticlockwise_square_1d] = rigid_data.num_rigids;
			rigid_data.num_rigids++;
			rigid_data.links[rigid_data.num_links] = math_intpair_create(player_rigid_number, rigid_data.grid[anticlockwise_square_1d]);
			rigid_data.num_links++;
		}
		if (state->floor[next_square_1d] == F_GRILL_HOT)
		{
			square_moving[player_1d] = false;
			cancel_moves_by_rigid(&rigid_data, square_moving, player_rigid_number, state->w * state->h);
		}
	}
	//build the symbol_start animation.
	{
		animation->symbol_start = *symbollocalanimation_create_internal(w * h, temp_memory);
		animation->symbol_end = *symbollocalanimation_create_internal(w * h, temp_memory);
		//allocate the starting values.
		symbollocalanimation_alloc_internal(state, &animation->symbol_start, w, h);
		symbollocalanimation_alloc_internal(state, &animation->symbol_end, w, h);
	}
	cancel_blocked_nonmerge_moves(&rigid_data, square_moving, pieces, state->piece_data, action, w, h);
	//if the player is moving and they are standing on top of a cold grill, turn the grill hot.
	{
	if (square_moving[player_1d] && state->floor[player_1d] == F_GRILL_COLD)
		state->floor[player_1d] = F_GRILL_HOT;
	}
	
	//calculate the animation for merging crates.
	//apply any merge moves, where a crate is merging with another crate.
	apply_merge_moves(state->piece_data, square_moving, pieces, action, w, h,animation);

	//get final values (e.g. after curses)
	animationmoveinfo_copy_from_gamestate_internal(&animation->ends, state);

	//calculate animation. For each thing, move it!
	animationmoveinfo_apply_movements(state,&animation->starts, &animation->ends, square_moving, action, w * h);

	//apply moves.
	apply_moves_to_layer_SLOW<int>(temp_memory, square_moving, pieces, w, h, action, P_NONE);
	apply_moves_to_layer_SLOW<PieceData>(temp_memory, square_moving, state->piece_data,w,h,action, gamestate_piecedata_make());
	//fin.
	journal->action_result = AR_ACTION_OCCURED;
	journal->maybe_animation = animation;
	return journal;
}
void gamestate_crumble(GameState* state)
{
	int len = state->w * state->h;
	for (int i = 0; i < len; i++)
	{
		if (state->piece[i] == P_CRUMBLE || state->piece[i] == P_CRATE)
			state->piece[i] = P_NONE;
	}
	for (int i = 0; i < len; i++)
	{
		if (state->floor[i] == F_START || state->floor[i] == F_TARGET)
			state->floor[i] = F_NONE;
	}
	for (int i = 0; i < len; i++)
	{
		if (state->floor[i] == F_EXIT)
			state->floor[i] = F_NONE;
	}
}
void gamestate_startup(GameState* state)
{
	gamestate_extrude_lurking_walls(state);

	//decide what exit we are going to treat as our king exit. 
	int exit_king_test = maybe_find_exit_or_return_neg1(state);
	//if we can't find ANY exit, that's a huge wierd thing, we need an exit-like to even enter the level, so this means crash, crash now.
	if (exit_king_test == -1)
	{
		crash_err("a gamestate was started up, but an exit couldn't be found. Uh oh.");
	}
	//Delete any exit that isn't that exit.
	for(int i = 0; i < state->w * state->h;i++)
	{
		if (i == exit_king_test)
			continue;
		if (state->floor[i] == F_EXIT || state->floor[i] == F_START || state->floor[i] == F_STAIRCASE_LEVELSTART)
		{
			state->floor[i] = F_NONE;
		}
	}
	//setup the entrance // exit.
	if(false)
	{
		bool found_entrance = false;
		bool found_exit = false;
		int entrance = 0;
		int exit = 0;
		for (int i = 0; i < state->w * state->h; i++)
		{
			if (is_level_start(state->floor[i]))
			{
				found_entrance = true;
				entrance = i;
				break;
			}
		}
		for (int i = 0; i < state->w * state->h; i++)
		{
				
			if (state->floor[i] == F_EXIT)
			{
				found_exit = true;
				exit = i;
				break;
			}
		}
		
		if (found_entrance && !found_exit)
		{
			state->floor[entrance] = F_EXIT;
		}

	}
}
void gamestate_extrude_lurking_walls(GameState* state)
{
	int len = state->w * state->h;
	for (int i = 0; i < len; i++)
	{
		if (state->floor[i] == F_LURKING_WALL)
		{
			state->piece[i] = P_WALL;
		}

	}
}



//scratch

//okay so the four mechanics I want to implement are:
	//push
	//pull
	//parrallel
	//merge 
	//using these 4 crates we can have a progression that looks like:
	//push / pull / parallel path, opening up gates to the next stage.
	//merge rule parallel cadence,
	//set parallel paths, that each lead towards keys that open the gate to the finale section. Paths are just cool cadences that rely on prior work.
	//finale puzzle.


