#pragma once

#include <string>
#include "Memory.h"
#include "GameState.h"


PIECE_IMAGE resource_cursed_direction_to_piece_sprite(CursedDirection dir);
FLOOR_IMAGE resource_floor_to_floor_sprite(Floor floor);
PIECE_IMAGE resource_piece_to_piece_sprite(Piece piece);
int resource_layer_value_to_layer_sprite_value(int layer_value, int layer_num);
std::string resource_load_puzzle_file(std::string filePath);
std::string resource_load_text_file(std::string filePath);
unsigned int resource_load_image_from_file_onto_gpu(std::string filePath);	//returns GLuint reference to gpu texture.
glm::vec4* resource_load_texcoords_floor(Memory* memory, Memory* tempMemory);
glm::vec4* resource_load_texcoords_pieces(Memory* memory, Memory* tempMemory);
