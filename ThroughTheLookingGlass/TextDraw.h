#pragma once

#include "Math.h"
#include "TrueFontCharacter.h"
const int FONT_CHARACTER_HEIGHT = 128;	//used to be 128.
const int FONT_ATLAS_WIDTH = 1024;
const int FONT_ATLAS_HEIGHT = 1024;
const int FONT_NUM_CHARACTERS = 128;

struct TextWrite {
	glm::mat4* string_matrix_cpu;
	glm::vec4* string_atlas_cpu;
	glm::vec4* string_color_cpu;
	TTF_Character* true_font_reference;
	AABB* text_positions;
	AABB* text_positions_normalized;
	int* current_number_drawn;
};

struct LineBounds
{
	float line_length;
	float y_top;
	float y_bot;
	float y_dist;
};
/// We go through all the steps of drawing text except drawing text, to figure out how much x distance the text will require to draw.
float draw_text_to_screen(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextWrite* info);



struct Line
{
	float center;
	float length;
	float top;
	float bot;

};

float percentage_between(float start, float end, float point);
float lerp(float start, float end, float percentage);
void draw_text_lines_at_same_size(GameSpaceCamera* draw_areas, const char** strings_to_draw, int num_to_draw, TextWrite* text_draw_info);
void draw_text_maximized_centered_to_screen(GameSpaceCamera camera, const char* string_to_draw, TextWrite* text_draw_info, float max_scale = 99999999999999.0f);

GameSpaceCamera area_get_grid_element(float x, float y, float width, float height, GameSpaceCamera area);
GameSpaceCamera* break_camera_into_columns(GameSpaceCamera area, int column, Memory* scope_memory);
GameSpaceCamera* break_camera_into_rows(GameSpaceCamera area, int rows, Memory* scope_memory);
GameSpaceCamera* break_camera_into_weighted_columns(GameSpaceCamera area, int* column_ratio, int length, Memory* scope_memory);
GameSpaceCamera* break_camera_into_weighted_rows(GameSpaceCamera area, int* row_ratio, int length, Memory* scope_memory);


//break the camera into six columns
//break the sixth camera into a 5/1/5
