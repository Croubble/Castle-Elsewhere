#pragma once
#pragma warning(disable:4996)
#include <string>
#include "Memory.h"

std::string load_puzzle_from_assets(std::string level_name);
int save_puzzle_file(std::string to_save);
std::string load_puzzle_file();
int save_game(std::string to_save);
void save_continue_file(std::string to_save);
std::string get_continue_file(Memory* temp_memory);