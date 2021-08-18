#pragma once
#include "Animation.h"
#include "Editor.h"
#include "Click.h"
#include "sprite.h"
#include "textureAssets.h"

/************************************************************/
/**********draw functions************************************/
void draw_layer_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, SpriteWrite* info, int layer_index);
void draw_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, AllWrite* info);
void draw_gamestates_outlines_to_gamespace(
	GameState** gamestates,
	IntPair* offsets,
	int length_function_input,
	SpriteWrite* info,
	int index);
void draw_palette(IntPair palete_screen_start,
	GameSpaceCamera camera_game,
	ViewPortCamera camera_viewport,
	EditorUIState* ui_state,
	int palete_length,
	GamestateBrush* palete,
	AllWrite* layer_draw);
void draw_line(glm::vec3 staircase_start, glm::vec3 staircase_end, SpriteWrite* info);
void draw_black_box_over_screen(GameSpaceCamera screen, SpriteWrite* info);
void draw_outline_to_gamespace(AABB outline, SpriteWrite* info);
void draw_button_to_gamespace(GameSpaceCamera draw_area, SpriteWrite* ui_draw, glm::vec4 color = glm::vec4(1,1,1,1));
void draw_ui_to_gamespace(GameSpaceCamera draw_area, int index, SpriteWrite* draw_info, glm::vec4 color = glm::vec4(1, 1, 1, 1));

