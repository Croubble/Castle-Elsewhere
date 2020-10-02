#pragma once
#include<iostream>
const int GAME_LEVEL_NAME_MAX_SIZE = 64;

const int MAX_EDITOR_ACTIONS = 100000;
const int MAX_NUMBER_GAMESTATES = 1000;
const float TIME_TO_DISPLAY_LEVEL_NAME = 1.5f;

enum SCENE_TYPE
{
	ST_EDITOR,
	ST_EDIT_LEVEL,
	ST_PLAY_WORLD,
	ST_PLAY_LEVEL,
	ST_SHOW_TEXT,
	ST_MENU,
	ST_COUNT
};

enum UI_SPRITE_NAME
{
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_CENTER,
	UP_ARROW,
	DOWN_ARROW,
	RIGHT_ARROW,
	LEFT_ARROW
};

//TYPEDEF FUNCTION DEFINITIONS

#define crash_err(...) full_crash_err(__VA_ARGS__, __FILE__, __LINE__)
void full_crash_err(const char* text, const char* file, int line);

