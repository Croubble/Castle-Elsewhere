#pragma once

#include <string>
#include "Memory.h"
#include "GameState.h"
#include "textureAssets.h"
typedef int (*get_atlas_number_from_name) (std::string);

textureAssets::PIECE resource_cursed_direction_to_piece_sprite(CursedDirection dir);


textureAssets::FLOOR resource_floor_to_sprite(Floor floor);
textureAssets::PIECE resource_piece_to_sprite(Piece piece);
int resource_layer_value_to_layer_sprite_value(int layer_value, int layer_num);
std::string resource_load_puzzle_file(std::string filePath);
std::string resource_load_text_file(std::string filePath);
unsigned int resource_load_image_from_file_onto_gpu(std::string filePath);	//returns GLuint reference to gpu texture.
