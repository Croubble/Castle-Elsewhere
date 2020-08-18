#include "GameState.h"
#include "Memory.h"
#include <iostream>
#include "Math.h"
#include "GenericGamestate.h"

//gamestate static functions, go right at top.
//TODO: Test apply_direction_moves_to_layer for the four functions below. They 1000000% have bugs.
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
static void apply_moves_to_layer_SLOW(Memory* temp_memory, bool* is_moving, int* layer, int w, int h, Direction action)
{
	//TODO: No longer need to call this, replace call with faster code, holy moly this is slow.
	int length = w * h;
	int* next_layer = (int*) memory_alloc(temp_memory, sizeof(int) * length);
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
			next_layer[z] = 0;
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
static inline bool can_be_pushed(int piece)
{
	return is_normal_crate(piece);
}
static inline bool can_be_pulled(int piece)
{
	return is_pull_crate(piece);
}
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
void gamestate_timemachine_reset(GamestateTimeMachine* timeMachine, Memory* scope_memory)
{
	int next_pos = timeMachine->num_gamestates_stored;
	GameState* next = &timeMachine->state_array[next_pos];
	GameState* old = &timeMachine->state_array[0];
	gamestate_clone_to_unitialized(old, next, scope_memory);
	timeMachine->num_gamestates_stored++;
}
Direction direction_reverse(Direction dir)
{
	if (dir == U)
		return D;
	if (dir == L)
		return R;
	if (dir == R)
		return L;
	if (dir == D)
		return U;
	return Direction::NO_DIRECTION;
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
					int old_1d = f2D(i, j,w,h);
					if (old_1d != z)
					{
						std::cout << "You know what it means" << std::endl;
						abort();
					}
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
	curse_gamestate(result->start_state);
	result->state_array = (GameState*)memory_alloc(memory, sizeof(GameState) * max_num_gamestates);
	//TODO: Get this function actually working correctly. Do eet.
	gamestate_clone_to_unitialized(result->start_state, &(result->state_array[0]), memory);
	//gamestate_clone_to(start_state, result->state_array);
	result->num_gamestates_stored = 1;
	result->max_gamestates_storable = max_num_gamestates;
	return result;
}

void gamestate_timemachine_undo(GamestateTimeMachine* timeMachine)
{
	if (timeMachine->num_gamestates_stored > 1)
		timeMachine->num_gamestates_stored--;
}
GameState* gamestate_timemachine_get_latest_gamestate(GamestateTimeMachine* timeMachine)
{
	return &timeMachine->state_array[timeMachine->num_gamestates_stored - 1];
}
GameStateAnimation* gamestate_timemachine_take_action(GamestateTimeMachine* timeMachine, Direction action, Memory* scope_memory, Memory* temp_memory)
{
	int next_pos = timeMachine->num_gamestates_stored;
	int old_pos = next_pos - 1;
	GameState* old = &timeMachine->state_array[old_pos];
	GameState* next = &timeMachine->state_array[next_pos];
	gamestate_clone_to_unitialized(old, next, scope_memory);
	GameStateAnimation* result = gamestate_action(next, action, temp_memory);
	timeMachine->num_gamestates_stored++;
	return result;
}

void gamestate_allocate_layers(GameState* result, Memory* memory, int w, int h)
{
	result->w = w;
	result->h = h;
	result->layers = (int**)memory_alloc(memory, sizeof(int*) * GAME_NUM_LAYERS);
	for (int i = 0; i < GAME_NUM_LAYERS; i++)
	{
		result->layers[i] = (int*)memory_alloc(memory, sizeof(int*) * w * h);
	}

	for (int i = 0; i < w * h; i++)
	{
		Floor f = (Floor)0;
		result->layers[LN_FLOOR][i] = f;

	}
	for (int i = 0; i < w * h; i++)
	{
		Piece p = (Piece)0;
		result->layers[LN_PIECE][i] = p;
	}
}
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
void gamestate_merge(GameState* left, GameState* right, GameState* output, IntPair left_merge_offset, IntPair right_merge_offset)
{
	int w_f = left->w;
	int h_f = left->h;
	int w_s = right->w;
	int h_s = right->h;
	int w_out = output->w;
	int h_out = output->h;

	for (int z = 0; z < GAME_NUM_LAYERS; z++)
	{
		for (int i = 0; i < w_f; i++)
			for (int j = 0; j < h_f; j++)
			{
				int index_in = f2D(i, j, w_f, h_f);
				int index_out = f2D(i + left_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->layers[z][index_out] = left->layers[z][index_in];
			}
		for (int i = 0; i < w_s; i++)
			for (int j = 0; j < h_s; j++)
			{
				int index_in = f2D(i, j, w_s, h_s);
				int index_out = f2D(i + right_merge_offset.x, j + left_merge_offset.y, w_out, h_out);
				output->layers[z][index_out] = left->layers[z][index_in];
			}
	}
}

bool gamestate_is_in_win_condition(GameState* state)
{
	int len = state->w * state->h;
	for (int i = 0; i < len; i++)
	{
		if (state->layers[LN_FLOOR][i] == F_START && !is_player(state->layers[LN_PIECE][i]))
		{
			return false;
		}

	}
	for (int i = 0; i < len; i++)
	{
		if (state->layers[LN_FLOOR][i] == F_TARGET && 
			!is_normal_crate(state->layers[LN_PIECE][i]) &&
			!is_pull_crate(state->layers[LN_PIECE][i]))
			return false;
	}
	return true;
}

bool is_player(int val)
{
	return val == P_PLAYER ||
		val == P_CURSED_PLAYER ||
		val == P_CURSED_PLAYERU ||
		val == P_CURSED_PLAYERR ||
		val == P_CURSED_PLAYERD ||
		val == P_CURSED_PLAYERL;
}

bool is_normal_crate(int val)
{
	return val == P_CRATE ||
		val == P_CURSED_CRATE || 
		val == P_CURSED_CRATED ||
		val == P_CURSED_CRATEU ||
		val == P_CURSED_CRATER ||
		val == P_CURSED_CRATEL;
}

bool is_pull_crate(int val)
{
	return val == P_PULL_CRATE ||
		val == P_CURSED_PULL_CRATE ||
		val == P_CURSED_PULL_CRATED ||
		val == P_CURSED_PULL_CRATEU ||
		val == P_CURSED_PULL_CRATEL ||
		val == P_CURSED_PULL_CRATER;
}

CursedDirection get_entities_cursed_direction(int val)
{
	if (val == P_CURSED_PLAYERD || val == P_CURSED_CRATED || val == P_CURSED_PULL_CRATED)
		return CursedDirection::DCURSED;
	if (val == P_CURSED_PLAYERU || val == P_CURSED_CRATEU || val == P_CURSED_PULL_CRATEU)
		return CursedDirection::UCURSED;
	if (val == P_CURSED_PLAYERR || val == P_CURSED_CRATER || val == P_CURSED_PULL_CRATER)
		return CursedDirection::RCURSED;
	if (val == P_CURSED_PLAYERL || val == P_CURSED_CRATEL || val == P_CURSED_PULL_CRATEL)
		return CursedDirection::LCURSED;
	if (val == P_CURSED_PLAYER || val == P_CURSED_CRATE || val == P_CURSED_PULL_CRATE)
		return CursedDirection::CURSED;
	return CursedDirection::NOTCURSED;
}

CursedDirection get_curseddirection_from_direction(Direction dir)
{
	if (dir == U)
		return CursedDirection::UCURSED;
	if (dir == R)
		return CursedDirection::RCURSED;
	if (dir == D)
		return CursedDirection::DCURSED;
	if (dir == L)
		return CursedDirection::LCURSED;
	return CursedDirection::NOTCURSED;
}

bool is_cursed(int entity)
{
	return !(get_entities_cursed_direction(entity) == CursedDirection::NOTCURSED);
}

bool curseaable(int entity)
{
	bool can_be_cursed = is_player(entity) || is_normal_crate(entity) || is_pull_crate(entity);
	bool cursed = is_cursed(entity);
	return can_be_cursed && !cursed;
}
int curse_entity(int entity_value, CursedDirection curse_to_apply)
{
	if (is_player(entity_value))
	{
		if (curse_to_apply == D)
			return P_CURSED_PLAYERD;
		if (curse_to_apply == R)
			return P_CURSED_PLAYERR;
		if (curse_to_apply == U)
			return P_CURSED_PLAYERU;
		if (curse_to_apply == L)
			return P_CURSED_PLAYERL;
		if (curse_to_apply == CursedDirection::CURSED)
			return P_CURSED_PLAYER;
		return P_PLAYER;
	}
	if (is_normal_crate(entity_value))
	{
		if (curse_to_apply == D)
			return P_CURSED_CRATED;
		if (curse_to_apply == R)
			return P_CURSED_CRATER;
		if (curse_to_apply == U)
			return P_CURSED_CRATEU;
		if (curse_to_apply == L)
			return P_CURSED_CRATEL;
		if (curse_to_apply == CursedDirection::CURSED)
			return P_CURSED_CRATE;
		return P_CRATE;
	}
	if (is_pull_crate(entity_value))
	{
		if (curse_to_apply == D)
			return P_CURSED_PULL_CRATED;
		if (curse_to_apply == U)
			return P_CURSED_PULL_CRATEU;
		if (curse_to_apply == R)
			return P_CURSED_PULL_CRATER;
		if (curse_to_apply == L)
			return P_CURSED_PULL_CRATEL;
		if (curse_to_apply == CursedDirection::CURSED)
			return P_CURSED_PULL_CRATE;
		return P_PULL_CRATE;
	}
	std::cout << "we appear to be trying to curse an entity that can't be cursed. This is would indicate an error in the code we wrote. Abort." << std::endl;
	abort();
}

GameState* gamestate_resize_with_allocate(GameState* input_state, Memory* output_memory, int output_w, int output_h, IntPair displacement_from_input_to_output)
{
	GameState* output_state = gamestate_create(output_memory, output_w, output_h);
	gamestate_resize(input_state, output_state, displacement_from_input_to_output);
	return output_state;
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
				for (int z = 0; z < GAME_NUM_LAYERS; z++)
				{
					output_state->layers[z][out_index] = input_state->layers[z][in_index];
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
		std::cout << "You fool! You tried to use gamestate_clone_to on an input_state that didn't have the correct amount of memory. This would be a memory leak. It's now a crash. Enjoy.";
		abort();
	}
	output_state->w = w;
	output_state->h = h;
	for(int i = 0; i < w;i++)
		for (int j = 0; j < h; j++)
		{
			for (int z = 0; z < GAME_NUM_LAYERS; z++)
			{
				int index = f2D(i, j, w, h);
				output_state->layers[z][index] = input_state->layers[z][index];
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
			for (int z = 0; z < GAME_NUM_LAYERS; z++)
			{
				output->layers[z][out_index] = input->layers[z][in_index];
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
			for (int z = 0; z < GAME_NUM_LAYERS; z++)
			{
				output->layers[z][out_index] = input->layers[z][in_index];
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
				result->layers[LN_PIECE][z] = P_WALL;
			}

	for (int z = 0; z < GAME_NUM_LAYERS; z++)
		for (int i = 0; i < old_w; i++)
			for (int j = 0; j < old_h; j++)
			{
				int in_index = f2D(i, j, old_w, old_h);
				int out_index = f2D(i + 1, j + 1, next_w, next_h);
				result->layers[z][out_index] = input->layers[z][in_index];
			}
	return result;
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
		if (state->layers[LN_FLOOR][target] != brush.floor)
		{
			state->layers[LN_FLOOR][target] = brush.floor;
			accept = true;
		}

	}
	if (brush.applyPiece)
	{
		int w = state->w;
		int h = state->h;
		int target = f2D(x, y, w, h);
		if (state->layers[LN_PIECE][target] != brush.piece)
		{
			state->layers[LN_PIECE][target] = brush.piece;
			accept = true;
		}

	}
	return accept;
}
GamestateBrush gamestate_brush_create(bool applyFloor, Floor floor, bool applyPiece, Piece piece)
{
	GamestateBrush result;
	result.applyFloor = applyFloor;
	result.floor = floor;
	result.applyPiece = applyPiece;
	result.piece = piece;
	return result;
}
bool gamestate_eq(GameState* left, GameState* right)
{
	return false;
}


AABB* gamestate_create_colliders(Memory* memory, GameState** states, IntPair* locations, int length)
{
	AABB* result = (AABB*)memory_alloc(memory, sizeof(AABB) * (length - 1));
	for (int i = 0; i < length; i++)
	{
		result[i].w = states[i]->w;
		result[i].h = states[i]->h;
	}
	for (int i = 0; i < length; i++)
	{
		result[i].x = locations[i].x;
		result[i].y = locations[i].y;
	}
	return result;
}

AABB* gamestate_create_colliders(Memory* memory, GameState** states, IntPair* locations, int length, int skip_index)
{
	AABB* result = (AABB*) memory_alloc(memory, sizeof(AABB) * (length - 1));
	for (int i = 0; i < skip_index; i++)
	{
		result[i].w = states[i]->w;
		result[i].h = states[i]->h;
	}
	for (int i = skip_index + 1; i < length; i++)
	{
		result[i - 1].w = states[i]->w;
		result[i - 1].h = states[i]->h;
	}
	for (int i = 0; i < skip_index; i++)
	{
		result[i].x = locations[i].x;
		result[i].y = locations[i].y;
	}
	for (int i = skip_index + 1; i < length; i++)
	{
		result[i - 1].x = locations[i].x;
		result[i - 1].y = locations[i].y;
	}
	return result;
}

void curse_gamestate(GameState* state)
{
	int w = state->w;
	int h = state->h;
	for (int i = 0; i < w * h; i++)
	{
		if (state->layers[LN_FLOOR][i] == F_CURSE)
			if (curseaable(state->layers[LN_PIECE][i]))
				state->layers[LN_PIECE][i] = curse_entity(state->layers[LN_PIECE][i],CursedDirection::CURSED);
	}
}

AnimationMoveInfo* gamestate_animationmoveinfo_create_internal(int num_elements, Memory* temp_memory)
{
	AnimationMoveInfo* result = (AnimationMoveInfo*)memory_alloc(temp_memory, sizeof(AnimationMoveInfo));
	result->pos = (IntPair*)memory_alloc(temp_memory, sizeof(IntPair) * num_elements);
	result->to_move = (Direction*)memory_alloc(temp_memory, sizeof(Direction) * num_elements);
	result->start_value = (int*)memory_alloc(temp_memory, sizeof(int) * num_elements);
	for (int i = 0; i < num_elements; i++)
	{
		result->pos[i] = math_intpair_create(0, 0);
		result->to_move[i] = Direction::NO_DIRECTION;
		result->start_value[i] = 0;
	}
	return result;
}
void animationmoveinfo_set_default_positions(AnimationMoveInfo* info, GameState* state)
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
void animationmoveinfo_copy_from_gamestate_internal(AnimationMoveInfo* info, GameState* state)
{
	int num_to_draw = state->w * state->h;
	for (int i = 0; i < num_to_draw; i++)
	{
		int next_val = state->layers[LN_PIECE][i];
		info->start_value[i] = next_val;
	}

}
void animationmoveinfo_apply_movements(GameState* state, AnimationMoveInfo* start,AnimationMoveInfo* end, bool* moves, Direction movement, int num_to_draw)
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
GameStateAnimation* gamestate_action(GameState* state, Direction action, Memory* temp_memory)
{

	const int w = state->w;
	const int h = state->h;
	const int num_elements = w * h;
	int* pieces = state->layers[LN_PIECE];

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
				//if we found a cursed player, cancel the action.
				if (get_entities_cursed_direction(pieces[i]) == action)
					return NULL;
				player_1d = i;
				player_2d = t2D(i,state->w,state->h);
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

	//for each moving player, if their is a crate in their movement direction, make that move too!
	{
		IntPair next_square = move_pos_wrapped_2d(player_2d, action, w, h);
		IntPair back_square = move_pos_wrapped_2d(player_2d, direction_reverse(action), w, h);
		int next_square_1d = f2D(next_square.x, next_square.y, w, h);
		int back_square_1d = f2D(back_square.x, back_square.y, w, h);
		int next_piece = pieces[next_square_1d];
		int back_piece = pieces[back_square_1d];
		if (can_be_pushed(next_piece))
			if(get_entities_cursed_direction(next_piece) != action)
				square_moving[next_square_1d] = true;
		if (can_be_pulled(back_piece))
			if (get_entities_cursed_direction(back_piece) != action)
				square_moving[back_square_1d] = true;
	}
	
	//curse anything standing on a curse.
	curse_gamestate(state);
	//block moves.
	cancel_blocked_moves(square_moving, pieces, action, w, h);

	//curse anything that is moving and is (already cursed / stepping onto a curse this turn)
	for (int i = 0; i < w * h; i++)
	{
		if (square_moving[i])
		{
			IntPair current_2d = t2D(i, w, h);
			IntPair next_2d = move_pos_wrapped_2d(current_2d, action, w, h);
			int next_1d = f2D(next_2d.x, next_2d.y, w, h);

			int next_floor_val = state->layers[LN_FLOOR][next_1d];
			int floor_val = state->layers[LN_FLOOR][i];
			int piece_val = state->layers[LN_PIECE][i];
			CursedDirection how_to_curse = get_curseddirection_from_direction(action);
			//if the floor is a curse, or the entity that moved is already cursed.
			if (next_floor_val == F_CURSE || get_entities_cursed_direction(piece_val) != CursedDirection::NOTCURSED)
			{
				//curse an entity.
				state->layers[LN_PIECE][i] = curse_entity(piece_val, how_to_curse);
			}
			if (next_floor_val == F_CLEANSE)
			{
				state->layers[LN_PIECE][i] = curse_entity(piece_val, CursedDirection::NOTCURSED);
			}
		}
	}

	//get final values (e.g. after curses)
	animationmoveinfo_copy_from_gamestate_internal(&animation->ends, state);

	//calculate animation
	animationmoveinfo_apply_movements(state,&animation->starts, &animation->ends, square_moving, action, w * h);

	//apply moves.
	apply_moves_to_layer_SLOW(temp_memory, square_moving, pieces, w, h, action);
	
	//fin.
	return animation;
}

void gamestate_crumble(GameState* state)
{
	int len = state->w * state->h;
	for (int i = 0; i < len; i++)
	{
		if (state->layers[LN_PIECE][i] == P_CRUMBLE || is_normal_crate(state->layers[LN_PIECE][i]) || is_pull_crate(state->layers[LN_PIECE][i]))
			state->layers[LN_PIECE][i] = P_NONE;
	}
	for (int i = 0; i < len; i++)
	{
		if (state->layers[LN_FLOOR][i] == F_START || state->layers[LN_FLOOR][i] == F_TARGET)
			state->layers[LN_FLOOR][i] = F_NONE;
	}
}
void gamestate_extrude_lurking_walls(GameState* state)
{
	int len = state->w * state->h;
	for (int i = 0; i < len; i++)
	{
		if (state->layers[LN_FLOOR][i] == F_LURKING_WALL)
		{
			state->layers[LN_PIECE][i] = P_WALL;
		}

	}
}