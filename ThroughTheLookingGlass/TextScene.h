#pragma once
#include "Constants.h"
#include <string>
#include "Memory.h"
struct TextScene
{
	char* to_display;
	float start_time;
	float end_time;
	SCENE_TYPE scene_to_revert_to;
};

TextScene* build_text_scene(std::string to_build, SCENE_TYPE last_scene,Memory* text_memory, float start_time, float display_time)
{
	memory_clear(text_memory);
	TextScene* result = (TextScene*) memory_alloc(text_memory, sizeof(TextScene));
	result->to_display = (char*) memory_alloc(text_memory, to_build.length() + 1);
	for (int i = 0; i < to_build.length(); i++)
		result->to_display[i] = to_build[i];
	result->to_display[to_build.length()] = NULL;
	result->start_time = start_time;
	result->end_time = start_time + display_time;
	result->scene_to_revert_to = last_scene;
	return result;
}

TextScene* level_popup(std::string level_name, Memory* text_memory, float current_time)
{
	return build_text_scene(level_name, SCENE_TYPE::ST_PLAY_LEVEL, text_memory, current_time, TIME_TO_DISPLAY_LEVEL_NAME);
}
