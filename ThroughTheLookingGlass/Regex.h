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

int resource_texturenames_to_pieceenum_names_internal(std::string s);
int resource_texturenames_to_floorsprite_enum_names_internal(std::string s);
int resource_texturenames_to_uisprite_enum_names_internal(std::string s);


AtlasData* GetAtlasPosition(Memory* temp_memory, std::string atlas, get_atlas_number_from_name func_atlas);