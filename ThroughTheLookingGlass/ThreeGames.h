#pragma once


#include "Math.h"

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
		GameState* old;
	};
	enum Action
	{
		Up,
		Right,
		Down,
		Left
	};

	void gamestate_update(GameState* to_update, Direction action, Memory* temp_memory);
}

