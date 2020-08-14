#pragma once

#include "Memory.h"
#include "GameState.h"
#include <iostream>
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
AtlasData* GetAtlasPosition(Memory* temp_memory, std::string atlas, int layer_name);
/*
AtlasData* GetAtlasPosition_old(Memory* memory, std::string atlas, int layer_name)
{
	AtlasData* result = (AtlasData*)memory_alloc(memory, sizeof(AtlasData));

	result->length = 0;
	try {
		//step 1: get the length.
		std::regex filename_regex("filename\": \"(.*)\",");
		{
			std::sregex_iterator next(atlas.begin(), atlas.end(), filename_regex);
			std::sregex_iterator end;
			while (next != end)
			{
				std::smatch match = *next;
				result->length++;
				next++;
			}
		}
		//step 1a: get the filenames converted into enums.
		result->enum_corrospoding_values = (int*)memory_alloc(memory, sizeof(int) * result->length);
		{
			int num_index = 0;
			std::sregex_iterator next(atlas.begin(), atlas.end(), filename_regex);
			std::sregex_iterator end;
			while (next != end)
			{
				std::smatch match = *next;
				std::string name = match.str(1);
				if(layer_name == LNR_FLOOR)
					result->enum_corrospoding_values[num_index] = resource_texturenames_to_floorsprite_enum_names_internal(name);
				else if(layer_name == LNR_PIECE)
					result->enum_corrospoding_values[num_index] = resource_texturenames_to_pieceenum_names_internal(name);
				else 
					result->enum_corrospoding_values[num_index] = resource_texturenames_to_floorsprite_enum_names_internal(name);
				num_index++;
				next++;
			}
		}

		result->positions = (Position*)memory_alloc(memory, sizeof(Position) * result->length);
		int index = 0;
		//step 2: get all the width, height, position, heights.
		std::regex data_regex("\"frame\": .\"x\":([0-9]+),\"y\":([0-9]+),\"w\":([0-9]+),\"h\":([0-9]+)");
		{
			std::sregex_iterator next(atlas.begin(), atlas.end(), data_regex);
			std::sregex_iterator end;
			while (next != end)
			{
				std::smatch match = *next;
				std::string x = match.str(1);
				std::string y = match.str(2);
				std::string w = match.str(3);
				std::string h = match.str(4);

				result->positions[index].x = std::stoi(x);
				result->positions[index].y = std::stoi(y);
				result->positions[index].w = std::stoi(w);
				result->positions[index].h = std::stoi(h);

				index++;
				next++;
			}
		}
		//step 3: get the meta data.
		std::regex meta_regex("\"size\": .\"w\":([0-9]*),\"h\":([0-9]*)");
		{
			std::smatch match;
			if (std::regex_search(atlas, match, meta_regex) && match.size() > 1) {
				std::string w = match.str(1);
				std::string h = match.str(2);

				result->metaWidth = std::stoi(w);
				result->metaHeight = std::stoi(h);
			}
		}

		return result;

	}
	catch (std::regex_error& e) {
		//ehh.
	}
	return nullptr;
}
*/