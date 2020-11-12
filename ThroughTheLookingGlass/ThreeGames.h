#pragma once


#include "Math.h"
#include "sprite.h"
namespace TripleCrate
{
	struct CratePowers
	{
		char push : 1;
		char pull : 1;
		char side : 1;
	};

	enum FloorEle
	{
		FNone,
		GrillCold,
		GrillHot,
		Target,
	};
	enum PieceEle
	{
		PNone,
		Wall,
		Player,
		Crate,
	};

	struct GameState
	{
		int w;
		int h;
		FloorEle* floor;
		PieceEle* piece;
		CratePowers* piece_data;
	};
	struct Journal
	{
		GameState* last_state;
	};
	enum Action
	{
		Up,
		Right,
		Down,
		Left
	};
	
	GameState* gamestate_clone(GameState* old, Memory* memory);
	void gamestate_update(GameState* to_update, Direction action, Memory* temp_memory);
	void draw_gamestate(GameState* to_draw, Journal* maybe_journal, IntPair* maybe_position_offset, glm::vec4* maybe_color_offset, SpriteWrite* out);
}

