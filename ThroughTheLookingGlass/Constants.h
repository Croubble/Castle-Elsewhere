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
	ST_MENU_KEYBIND,
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

// settings
const int SCREEN_STARTING_WIDTH = 800;
const int SCREEN_STARTING_HEIGHT = 600;
const float SCREEN_RATIO = ((float)SCREEN_STARTING_WIDTH / (float)SCREEN_STARTING_HEIGHT);
const int MAX_NUM_FLOOR_SPRITES = 5000;


const float CAMERA_LERP_TIME = 0.8f;
const float CAMERA_ZOOM_SPEED = 3.0f;
const float CAMERA_MOVE_MULT = 0.02f;
//const float MAX_ZOOM = 3.0f;
const float MAX_ZOOM = 2.0f;
const float MAX_ZOOM_OUT_TO_STILL_DISPLAY_OUTLINES = 22.0f;
const int MAX_NUM_DOTTED_SPRITES = 200;
const int MAX_NUM_CHARACTERS = 200;
const float OUTLINE_DRAW_SIZE = 0.3f;



const int max_actions_in_puzzle = 1024;

const float WAIT_BETWEEN_PLAYER_MOVE_REPEAT = 0.13f;
const float DRAW_TITLE_TIME = 2.0f;


//TYPEDEF FUNCTION DEFINITIONS

#define crash_err(...) do {full_crash_err(__VA_ARGS__, __FILE__, __LINE__);} while(0)
void full_crash_err(const char* text, const char* file, int line);
