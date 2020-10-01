#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "PreludeIncludes.h"
#include <GLFW/glfw3.h>
#include "Memory.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm\ext\vector_float4.hpp>
#include "GameState.h"
#include <string>
#include "Regex.h"
#include "Resource.h"

PIECE_IMAGE resource_cursed_direction_to_piece_sprite(CursedDirection dir)
{
	if (dir == CursedDirection::RCURSED)
		return PIECE_IMAGE::PI_STOPR;
	if (dir == CursedDirection::UCURSED)
		return PIECE_IMAGE::PI_STOPU;
	if (dir == CursedDirection::LCURSED)
		return PIECE_IMAGE::PI_STOPL;
	if (dir == CursedDirection::DCURSED)
		return PIECE_IMAGE::PI_STOPD;
	return PIECE_IMAGE::PI_NONE;
}

FLOOR_IMAGE resource_floor_to_floor_sprite(Floor floor)
{
	return (FLOOR_IMAGE)floor;
}

PIECE_IMAGE resource_piece_to_piece_sprite(Piece piece)
{
	if (is_player(piece))
	{
		if(is_cursed(piece))
			return PI_CURSED_PLAYER;
		return PI_PLAYER;
	}
	if (is_normal_crate(piece))
	{
		if (is_cursed(piece))
			return PI_CURSED_CRATE;
		return PI_CRATE;
	}
	if (is_pull_crate(piece))
	{
		if(is_cursed(piece))
			return PI_CURSED_PULL_CRATE;
		return PI_PULL_CRATE;
	}
	return (PIECE_IMAGE)piece;
}

int resource_layer_value_to_layer_sprite_value(int layer_value, int layer_num)
{
	if (layer_num == LN_FLOOR)
		return resource_floor_to_floor_sprite((Floor)layer_value);
	else
		return resource_piece_to_piece_sprite((Piece)layer_value);
}

std::string resource_load_puzzle_file(std::string puzzlename)
{
	std::string final_path = "assets/puzzles/" + puzzlename + ".puzzle";
	std::cout << "final path: " << final_path << std::endl;
	std::ifstream myFile(final_path);
	std::stringstream myStream;
	myStream << myFile.rdbuf();
	myFile.close();
	std::string result = myStream.str();
	return result;
}
std::string resource_load_text_file(std::string filePath)
{
	std::ifstream myFile("assets/" + filePath);
	std::stringstream myStream;
	myStream << myFile.rdbuf();
	myFile.close();
	std::string result = myStream.str();
	return result;
}

unsigned int resource_load_image_from_file_onto_gpu(std::string file_path)
{
	std::string asset_file_path = "assets/" + file_path;
	unsigned int result;
	glGenTextures(1, &result);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, result);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

	int width;
	int height;
	int num_of_channels;
	unsigned char* data = stbi_load(asset_file_path.c_str(), &width, &height, &num_of_channels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Texture load failed, name of file:" << file_path.c_str() << "looks like the programs gonna crash now" << std::endl;
	}
	stbi_image_free(data);

	return result;
}

glm::vec4* resource_load_texcoords_pieces(Memory* memory, Memory* tempMemory)
{
	std::string file = resource_load_text_file("FinalPiece.json");
	AtlasData* data = GetAtlasPosition(memory, file, LNR_PIECE);
	glm::vec4* result = (glm::vec4*) memory_alloc(memory, sizeof(glm::vec4) * data->length);

	int totalW = data->metaWidth;
	int totalH = data->metaHeight;
	for (int i = 0; i < data->length; i++)
	{
		Position p = data->positions[i];
		//flip the starting coordinates into our actual coordinate system.
		p.y = totalH - 1 - p.y;
		p.y -= p.h - 1;

		//turn w/h into x2/y2
		p.w += p.x - 1;
		p.h += p.y - 1;

		int next_index = data->enum_corrospoding_values[i];
		//convert from int to floats between 0 and 1
		result[next_index].x = (float)p.x / (totalW - 1);
		result[next_index].y = (float)p.y / (totalH - 1);
		result[next_index].z = (float)p.w / (totalW - 1);
		result[next_index].w = (float)p.h / (totalH - 1);
	}

	return result;
}

glm::vec4* resource_load_texcoords_ui(Memory* memory, Memory* tempMemory)
{
	std::string file = resource_load_text_file("FinalUI.json");
	AtlasData* data = GetAtlasPosition(memory, file, LNR_FLOOR);

	glm::vec4* result = (glm::vec4*) memory_alloc(memory, sizeof(glm::vec4) * data->length);
	int totalH = data->metaHeight;
	int totalW = data->metaWidth;

	for (int i = 0; i < data->length; i++)
	{
		Position p = data->positions[i];
		//flip the starting coordinates into our actual coordinate system.
		p.y = totalH - 1 - p.y;
		p.y -= p.h - 1;

		//turn w/h into x2/y2
		p.w += p.x - 1;
		p.h += p.y - 1;

		int next_index = data->enum_corrospoding_values[i];
		//convert from int to floats between 0 and 1
		result[next_index].x = (float)p.x / (totalW - 1);
		result[next_index].y = (float)p.y / (totalH - 1);
		result[next_index].z = (float)p.w / (totalW - 1);
		result[next_index].w = (float)p.h / (totalH - 1);
	}

	return result;
}
glm::vec4* resource_load_texcoords_floor(Memory* memory, Memory* tempMemory)
{
	std::string file = resource_load_text_file("FinalFloor.json");
	AtlasData* data = GetAtlasPosition(memory, file, LNR_FLOOR);

	glm::vec4* result = (glm::vec4*) memory_alloc(memory, sizeof(glm::vec4) * data->length);
	int totalH = data->metaHeight;
	int totalW = data->metaWidth;

	for (int i = 0; i < data->length; i++)
	{
		Position p = data->positions[i];
		//flip the starting coordinates into our actual coordinate system.
		p.y = totalH - 1 - p.y;
		p.y -= p.h - 1;

		//turn w/h into x2/y2
		p.w += p.x - 1;
		p.h += p.y - 1;

		int next_index = data->enum_corrospoding_values[i];
		//convert from int to floats between 0 and 1
		result[next_index].x = (float) p.x / (totalW - 1);
		result[next_index].y = (float) p.y / (totalH - 1);
		result[next_index].z = (float) p.w / (totalW - 1);
		result[next_index].w = (float) p.h / (totalH - 1);
	}

	return result;
}
//old

