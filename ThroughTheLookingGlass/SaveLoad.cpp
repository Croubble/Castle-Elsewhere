#include "SaveLoad.h"
#include "Memory.h"
//#include <Windows.h>
#ifndef EMSCRIPTEN
#include "tinyfiledialogs.h"
#endif
#include <iostream>

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

#ifndef EMSCRIPTEN
std::string load_puzzle_file()
{
	char const* lTheOpenFileName;
	FILE* lIn;
	char const* lFilterPatterns[2] = { "*.puzzle", "*.puz" };

	lTheOpenFileName = tinyfd_openFileDialog(
		"Choose a level to load",
		"D:\\Puzzles\\myname",
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

	lIn = fopen(lTheOpenFileName, "r");
	if (!lIn)
	{
		tinyfd_messageBox(
			"Error",
			"Can not open this file in read mode",
			"ok",
			"error",
			1);
		return "";
	}
	//TODO: figure out the fastest way to read a file into memory.
	const int buffer_size = 100000;
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
		if (i == 195)
			std::cout << "oohwee!" << std::endl;
		char c = (char) fgetc(lIn);
		buffer[i] = c;
		i++;
	}
	if (i >= buffer_size)
	{
		std::cout << "our level files are finally to big for our buffer in saveload load function. Go in, implement it using memory_alloc instead of the current hacky way." << std::endl;
		abort();
	}
	buffer[i - 1] = '\0';
	fclose(lIn);
	return std::string(buffer);
}
#else
std::string load_puzzle_file()
{
	return "";
}
#endif