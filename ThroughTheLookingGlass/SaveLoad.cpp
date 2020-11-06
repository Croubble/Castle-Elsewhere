#include "SaveLoad.h"
#include "Memory.h"
//#include <Windows.h>
#ifndef EMSCRIPTEN
#include "tinyfiledialogs.h"
#endif
#include <iostream>
#include "Constants.h"
std::string load_puzzle_from_assets(std::string level_name)
{
	std::cout << "level file we are trying to open:" << level_name << std::endl;
	FILE* lIn;
	lIn = fopen(level_name.c_str(), "r");
	if (!lIn)
	{
		std::cout << "function load_puzzle_from_assets has failed to find the path specified. (SaveLoad.cpp, line 17). Check it out!" << std::endl;
	}
	const int buffer_size = 50000;
	char buffer[buffer_size];

	int flen =
		buffer[0] = '\0';
	int i = 0;
	for (int z = 0; z < buffer_size; z++)
	{
		buffer[z] = '\0';
	}
	while (!feof(lIn) && i < buffer_size)
	{
		char c = (char)fgetc(lIn);
		buffer[i] = c;
		i++;
	}
	if (i >= buffer_size)
	{
		crash_err("our level files are finally to big for our buffer in saveload load function. Go in, implement it using memory_alloc instead of the current hacky way.");
	}
	if(i > 0)
		buffer[i - 1] = '\0';
	fclose(lIn);
	return std::string(buffer);
}
#ifdef EMSCRIPTEN
int save_puzzle_file(std::string to_save)
{
	return 1;
} 
#else
int save_puzzle_file(std::string to_save)
{
	char const* lTheSaveFileName;
	char const* lWillBeGraphicMode;
	FILE* lIn;
	char lBuffer[1024];
	char const* lFilterPatterns[2] = { "*.puzzle", "*.puz" };

	lWillBeGraphicMode = tinyfd_inputBox("tinyfd_query", NULL, NULL);

	if (lWillBeGraphicMode)
	{
		strcpy(lBuffer, "graphic mode: ");
	}
	else
	{
		strcpy(lBuffer, "console mode: ");
	}



	strcat(lBuffer, tinyfd_response);
	//tinyfd_messageBox(lThePassword, lBuffer, "ok", "info", 0);

	lTheSaveFileName = tinyfd_saveFileDialog(
		"let us save this world",
		"world.puzzle",
		2,
		lFilterPatterns,
		NULL);

	if (!lTheSaveFileName)
	{
		tinyfd_messageBox(
			"Error",
			"Save file name is NULL",
			"ok",
			"error",
			1);
		return 1;
	}

	lIn = fopen(lTheSaveFileName, "w");
	if (!lIn)
	{
		tinyfd_messageBox(
			"Error",
			"Can not open this file in write mode",
			"ok",
			"error",
			1);
		return 1;
	}
	fputs(to_save.c_str(), lIn);
	fclose(lIn);
	return 0;
}
#endif

void save_continue_file(std::string to_save)
{
	FILE* lIn;
	const char* save_name = ".\\assets\\puzzles\\save1.game";
	lIn = fopen(save_name, "w");
	if (!lIn)
	{
		crash_err("tried to save continue file. It failed!");
	}
	fputs(to_save.c_str(), lIn);
	fclose(lIn);
}

std::string get_continue_file(Memory* temp_memory)
{
	const char* to_load = ".\\assets\\puzzles\\save1.game";
	FILE* lIn = fopen(to_load, "r");
	if (!lIn)
	{
		std::cout << "no continue file to load" << std::endl;
		return	"";
	}
	int size = 0;
	while(!feof(lIn))
	{
		fgetc(lIn);
		size++;
	}
	fseek(lIn, 0, SEEK_SET); // seek back to beginning of file
	
	char* result = (char*) memory_alloc(temp_memory, sizeof(char) * (size + 1));
	for (int i = 0; i < size; i++)
	{
		result[i] = (char)fgetc(lIn);
		if (feof(lIn))
		{
			result[i] = NULL;
			break;
		}
	}
	result[size] = NULL;
	std::cout << result << std::endl;
	return std::string(result);
}
#ifndef EMSCRIPTEN
std::string load_puzzle_file()
{
	char const* lTheOpenFileName;
	FILE* lIn;
	char const* lFilterPatterns[2] = { "*.puzzle", "*.puz" };

	lTheOpenFileName = tinyfd_openFileDialog(
		"Choose a level to load",
		".\\assets\\puzzles\\myname",
		//"D:\\Puzzles\\myname",
		2,
		lFilterPatterns,
		NULL,
		0);

	if (!lTheOpenFileName)
	{
		tinyfd_messageBox(
			"Error",
			"Open file name is NULL",
			"ok",
			"error",
			1);
		return "";
	}
	return load_puzzle_from_assets(lTheOpenFileName);
}
#else
std::string load_puzzle_file()
{
	return "";
}
#endif