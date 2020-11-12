#include "ThreeGames.h"
#include "GenericUtil.h"
static __forceinline IntPair move_pos_wrapped_2d_up(IntPair pos, int w, int h)
{
	pos.y += 1;
	pos.y -= (pos.y >= h) * h;
	return pos;
}
static __forceinline IntPair move_pos_wrapped_2d_right(IntPair pos, int w, int h) {
	pos.x += 1;
	pos.x -= (pos.x >= w) * w;
	return pos;
}
static __forceinline IntPair move_pos_wrapped_2d_down(IntPair pos, int w, int h) {
	pos.y -= 1;
	pos.y += (pos.y < 0) * h;
	return pos;
}
static __forceinline IntPair move_pos_wrapped_2d_left(IntPair pos, int w, int h) {
	pos.x -= 1;
	pos.x += (pos.x < 0) * w;
	return pos;
}
static __forceinline IntPair move_pos_wrapped_2d(IntPair pos, Direction direction, int w, int h)
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
	int* next_layer = (int*)memory_alloc(temp_memory, sizeof(int) * length);
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

TripleCrate::GameState* TripleCrate::gamestate_clone(GameState* old, Memory* memory)
{
	GameState* result = (GameState*)memory_alloc(memory, sizeof(GameState));
	int w = old->w;
	int h = old->h;
	result->w = old->w;
	result->h = old->h;
	int len = old->w * old->h;
	result->floor = (FloorEle*)memory_alloc(memory, sizeof(sizeof(FloorEle) * len));
	result->piece = (PieceEle*)memory_alloc(memory, sizeof(sizeof(PieceEle) * len));
	for (int i = 0; i < len; i++)
	{
		result->floor[i] = old->floor[i];
	}
	for (int i = 0; i < len; i++)
	{
		result->piece[i] = old->piece[i];
	}
	return result;
}

void TripleCrate::gamestate_update(GameState* to_update, Direction action, Memory* temp_memory)
{
	//algorithm startup calculations.
	int player_pos = find_element(to_update->piece, PieceEle::Player, to_update->w * to_update->h);
	IntPair player_pos_2d = t2D(player_pos, to_update->w, to_update->h);

	bool* movement = (bool*)memory_alloc(temp_memory, sizeof(bool) * to_update->w * to_update->h);
	for (int i = 0; i < to_update->w * to_update->h; i++)
		movement[i] = false;
	movement[player_pos] = true;

	IntPair next_pos_2d = move_pos_wrapped_2d(player_pos_2d, action, to_update->w, to_update->h);
	int next_pos = f2D(next_pos_2d.x, next_pos_2d.y, to_update->w, to_update->h);
	IntPair back_pos_2d = move_pos_wrapped_2d(player_pos_2d, direction_reverse(action), to_update->w, to_update->h);
	int back_pos = f2D(back_pos_2d.x, back_pos_2d.y, to_update->w, to_update->h);
	IntPair side_pos_a_2d = move_pos_wrapped_2d(player_pos_2d, direction_rotate_clockwise(action), to_update->w, to_update->h);
	int side_pos_a = f2D(side_pos_a_2d.x, side_pos_a_2d.y, to_update->w, to_update->h);
	IntPair side_pos_b_2d = move_pos_wrapped_2d(player_pos_2d, direction_rotate_anti(action), to_update->w, to_update->h);
	int side_pos_b = f2D(side_pos_b_2d.x, side_pos_b_2d.y, to_update->w, to_update->h);

	//IF the player would move into a hot grill, cancel move.
	{
		if (to_update->floor[next_pos] == FloorEle::GrillHot)
		{
			return;
		}
	}

	//IF the player is standing on a cold grill, mark the cold grill.
	int grill_to_activate = -1;
	{
		if (to_update->floor[player_pos] == FloorEle::GrillCold)
			grill_to_activate = player_pos;
	}

	//add any pushed, pulled, or side crates to the movement array.
	{
		if (to_update->piece[next_pos] == PieceEle::Crate && to_update->piece_data[next_pos].push)
		{
			movement[next_pos] = true;
		}
		if (to_update->piece[back_pos] == PieceEle::Crate && to_update->piece_data[back_pos].pull)
		{
			movement[back_pos] = true;
		}
		if (to_update->piece[side_pos_a] == PieceEle::Crate && to_update->piece_data[side_pos_a].pull)
		{
			movement[side_pos_a] = true;
		}
		if (to_update->piece[side_pos_b] == PieceEle::Crate && to_update->piece_data[side_pos_b].pull)
		{
			movement[side_pos_b] = true;
		}
	}

	//apply the movement array to the piece state.
	apply_moves_to_layer_SLOW(temp_memory, movement, (int*)to_update->piece, to_update->w, to_update->h, action);
	apply_moves_to_layer_SLOW(temp_memory, movement, (int*)to_update->piece_data, to_update->w, to_update->h, action);
	//if a marked grill has no player on it, transform it into a hot grill.
	if (grill_to_activate >= 0 && to_update->piece[player_pos] != PieceEle::Player)
		to_update->floor[grill_to_activate] = FloorEle::GrillHot;
}

void TripleCrate::draw_gamestate(GameState* to_draw, Journal* maybe_journal, IntPair* maybe_position_offset, glm::vec4* maybe_color_offset, SpriteWrite* out)
{
	int w = to_draw->w;
	int h = to_draw->h;
	//draw the floor.
	{
		//map every element name to a sprite to draw (or -1 if no sprite to draw). 
		int z = 0;
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++, z++)
			{

			}
	}
}
