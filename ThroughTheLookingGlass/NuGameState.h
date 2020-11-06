#pragma once
#include <stdlib.h>     /* malloc, free, rand */

const int max_rigid_num = 64;
struct Immobile
{
	char floor_value : 3; //empty, wall, player_escape, player_target, crate_target.
	char gate_connector_value : 5;
};

struct Mobile
{
	char piece_value : 2; //nothing, player, crate.
	char push : 1;	//whether the crate can be pushed.
	char pull : 1; //whether the crate can be pulled.
	char side : 1; //wheter the crate can be side pulled.
	char rigid_value : 3; //what rigid body value this piece has. 0 is none, 1-7 is available (max 7 diff rigid crates).
};

enum Floor
{
	F_EMPTY,
	F_TARGET_PLAYER,
	F_TARGET_CRATE,
	F_GRILL_COLD,
	F_GRILL_HOT,
	F_GATE,
	F_COUNT
};
struct Floor_Data
{
	Floor name;
	char rigid_id;
};
enum Piece
{
	P_EMPTY,
	P_WALL,
	P_PLAYER,
	P_CRATE,
	P_GATE_RAISED,
};
struct Piece_data
{
	Piece name;
	bool pull;
	bool push;
	bool side;
	bool slide;
	bool side_sticky[4];
	unsigned char rigid_id;
	unsigned char fused_rigid_id;
	
};
struct NuGameState
{
	int w;
	int h;
	Floor_Data* floor;
	Piece_data* piece;
};

struct IntPair
{
	int x;
	int y;
};
int move_left_1d(int pos, int w, int h)
{
	return pos - h + (pos > 0) * w * h;
}
int move_right_1d(int pos, int w, int h)
{
	return pos + h - (pos >= w * h) * w * h;
}
int move_up_1d(int pos, int w, int h)
{
	return pos + 1 - h * ((pos + 1) % h == 0);
}
int move_down_1d(int pos, int w, int h)
{
	return pos - 1 + h * (pos % h == 0);
}
int move_left_2d(int x, int y, int w, int h)
{
	return x - 1 + w * (x < 0);	//wrap value if x is less than 0.
}
int move_right_2d(int x, int y, int w, int h)
{
	return x + 1 - w * (x >= w); //wrap value if x has overflown across the border.
}
int move_up_2d(int x, int y, int w, int h)
{
	return y + 1 - h * (y >= h);
}
int move_down_2d(int x, int y, int w, int h)
{
	return y - 1 + h * (y < 0);
}
inline int f2D(int x, int y, int w, int h)
{
	return y + x * h;
}

//from 1 dimension to 2 dimensions.
inline IntPair t2D(int i, int w, int h)
{
	IntPair result;
	result.x = i / h;
	result.y = i % h;
	return result;
}

void UpdateGameState(int player_move_x, int player_move_y, NuGameState* state, int length)
{
	int arr_len = state->w * state->h;
	//for each mobile element in array, apply the player move action to that element.
	for (int i = 0; i < length; i++)
	{
		//1:find the players current position.
		int player_pos;
		for (int i = 0; i < arr_len; i++)
		{
			if (state->piece[i].name == Piece::P_PLAYER)
			{
				player_pos = i;
				break;
			}
		}
		if (player_pos == -1)
			return;
		
		const IntPair player_pos_2d = t2D(player_pos,state->w,state->h);

		//2a:apply any pull moves.
		{
			//get next square.
			//if those squares contain "pull" crates, mark them, and their rigidbody numbers, as moving.
		}
		//2b:apply any push moves.
		{
			//get behind square.
			//if those squares contain "push" crates, mark them, and their rigidbody numbers, as moving.
		}
		//2c:apply any side moves.
		{
			//get both parallel squares.
			//if those squares contain "side" crates, mark them, and their rigidbody numbers, as moving.
		}
		//3:propegate rigidbody moves.
		{
			//for each element, if its a crate or a player, and its rigidbody isn't 0, check if its rigid matches any moving rigids. if it does, make it move.
		}
		//4:for each moving square, cancel its movement if its blocked. If any square's movement was cancelled, repeat this step.
		{
			bool done = false;
			int rigid_cancelled_length = 0;
			int rigid_numbers[max_rigid_num];
			for (int i = 0; i < max_rigid_num; i++)
			{
				rigid_numbers[i] = 0;
			}
			while (!done)
			{
				done = true;
				int current_x = 0;
				int current_y = 0;
				int next_x = player_move_x + (player_move_x < 0) * state->w * state->h;
				int next_y = player_move_y + (player_move_y < 0) * state->h;
				for (int i = 0; i < arr_len; i++)
				{
				}
			}
		}
		//5:apply any remaining moves.
		//6:update crate link.

	}
}

