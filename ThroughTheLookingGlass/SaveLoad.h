#pragma once
#pragma warning(disable:4996)
#include <string>


std::string load_puzzle_from_assets(std::string level_name);
int save_puzzle_file(std::string to_save);
std::string load_puzzle_file();