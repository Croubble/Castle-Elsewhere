#pragma once

#include "Memory.h"
#include "GameState.h"
#include <iostream>
#include "Resource.h"
enum LAYERNAMEREGEX
{
	LNR_FLOOR,
	LNR_PIECE,
	LNR_COUNT
};
struct Position
{
	int x;
	int y;
	int w;
	int h;
};

struct str
{
	int length;
	char* val;
};

struct AtlasData
{
	int metaWidth;
	int metaHeight;
	int length;
	int* enum_corrospoding_values;
	Position* positions;
};



AtlasData* GetAtlasPosition(Memory* temp_memory, std::string atlas, get_atlas_number_from_name func_atlas);