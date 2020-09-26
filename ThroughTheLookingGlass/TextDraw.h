#pragma once

#include "Math.h"
const int FONT_CHARACTER_HEIGHT = 128;	//used to be 48.
const int FONT_ATLAS_WIDTH = 1024;
const int FONT_ATLAS_HEIGHT = 1024;
const int FONT_NUM_CHARACTERS = 128;

struct TextDrawInfo {
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
float get_horizontal_space_consumed_by_text(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextDrawInfo* info)
{
	//TODO: get info.current_number_drawn to replace the other number we were using.
	glm::vec3 next_position = start_position;
	int draw = *info->current_number_drawn;
	for (; c_string[0]; c_string++)
	{
		char c = c_string[0];

		//handle the position of the character.
		float horizontal_space = info->text_positions[c].w / FONT_CHARACTER_HEIGHT;

		float x_start = (next_position.x + info->true_font_reference[c].Bearing.x) / float(FONT_CHARACTER_HEIGHT);
		float y_start = (next_position.y - (info->true_font_reference[c].Size.y - info->true_font_reference[c].Bearing.y)) / float(FONT_CHARACTER_HEIGHT);
		float w = (info->true_font_reference[c].Size.x) / float(FONT_CHARACTER_HEIGHT);
		float h = info->true_font_reference[c].Size.y / float(FONT_CHARACTER_HEIGHT);

		//we always increment the draw foward, even when we don't draw stuff like an empty space.
		float x_pixels_in_gamespace = (info->true_font_reference[c].advance >> 6) / float(FONT_CHARACTER_HEIGHT);
		next_position.x += (x_pixels_in_gamespace * scale.x);

	}
	*info->current_number_drawn = draw;
	return next_position.x - start_position.x;
}

LineBounds get_linebounds_space_consumed_by_text(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextDrawInfo* info)
{
	//TODO: get info.current_number_drawn to replace the other number we were using.
	glm::vec3 next_position = start_position;
	LineBounds result;
	result.y_bot = 0;
	result.y_top = 0;
	result.y_dist = 0;
	int draw = *info->current_number_drawn;
	for (; c_string[0]; c_string++)
	{
		char c = c_string[0];

		//handle the position of the character.
		float horizontal_space = info->text_positions[c].w / FONT_CHARACTER_HEIGHT;

		float x_start = (next_position.x + info->true_font_reference[c].Bearing.x) / float(FONT_CHARACTER_HEIGHT);
		float y_start = (next_position.y - (info->true_font_reference[c].Size.y - info->true_font_reference[c].Bearing.y)) / float(FONT_CHARACTER_HEIGHT);
		float w = (info->true_font_reference[c].Size.x) / float(FONT_CHARACTER_HEIGHT);
		float h = info->true_font_reference[c].Size.y / float(FONT_CHARACTER_HEIGHT);

		//we always increment the draw foward, even when we don't draw stuff like an empty space.
		float x_pixels_in_gamespace = (info->true_font_reference[c].advance >> 6) / float(FONT_CHARACTER_HEIGHT);
		next_position.x += (x_pixels_in_gamespace * scale.x);
		
		result.y_bot = minf(result.y_bot, y_start);
		result.y_top = maxf(result.y_top, y_start + h);
		result.y_dist = maxf(result.y_dist, h);
	}
	*info->current_number_drawn = draw;
	result.line_length = next_position.x - start_position.x;
	return result;
}

float draw_text_to_screen(glm::vec3 start_position, glm::vec2 scale, const char* c_string, TextDrawInfo* info)
{
	//TODO: get info.current_number_drawn to replace the other number we were using.
	glm::vec3 next_position = start_position;
	int draw = *info->current_number_drawn;
	for (; c_string[0]; c_string++)
	{
		char c = c_string[0];

		//handle the position of the character.
		float horizontal_space = info->text_positions[c].w / FONT_CHARACTER_HEIGHT;

		float x_start = (next_position.x + info->true_font_reference[c].Bearing.x) / float(FONT_CHARACTER_HEIGHT);
		float y_start = (next_position.y - (info->true_font_reference[c].Size.y - info->true_font_reference[c].Bearing.y) * scale.y) / float(FONT_CHARACTER_HEIGHT);
		float w = (info->true_font_reference[c].Size.x) / float(FONT_CHARACTER_HEIGHT);
		float h = info->true_font_reference[c].Size.y / float(FONT_CHARACTER_HEIGHT);

		if (c != ' ')
		{
			glm::vec3 start_position = glm::vec3(next_position.x + x_start, next_position.y + y_start, next_position.z);
			info->string_matrix_cpu[draw] = glm::mat4(1.0f);
			info->string_matrix_cpu[draw] = glm::translate(info->string_matrix_cpu[draw], start_position);
			info->string_matrix_cpu[draw] = glm::scale(info->string_matrix_cpu[draw], glm::vec3(w * scale.x, h * scale.y, 1));
			info->string_color_cpu[draw] = glm::vec4(0.5, 0.2, 0.3, 1);
			//handle what character should be drawn.
			AABB box = info->text_positions_normalized[c];
			glm::vec4 to_atlas_output = glm::vec4(box.x, box.y, box.x + box.w, box.y + box.h);
			info->string_atlas_cpu[draw] = to_atlas_output;

			draw++;
		}

		//we always increment the draw foward, even when we don't draw stuff like an empty space.
		float x_pixels_in_gamespace = (info->true_font_reference[c].advance >> 6) / float(FONT_CHARACTER_HEIGHT);
		next_position.x += (x_pixels_in_gamespace * scale.x);

	}
	*info->current_number_drawn = draw;
	return next_position.x - start_position.x;
}



struct Line
{
	float center;
	float length;
	float top;
	float bot;

};

float percentage_between(float start, float end, float point)
{
	return (point - start) / (end - start);
}
float lerp(float start, float end, float percentage)
{
	return percentage * end + (1.0f - percentage) * start;
}
void draw_text_maximized_centered_to_screen(GameSpaceCamera camera, char* string_to_draw, TextDrawInfo* text_draw_info)
{
	//draw the text.
	glm::vec3 screen_center = glm::vec3((camera.left + camera.right) / 2.0f, (camera.up + camera.down) / 2.0f, 15);
	
	LineBounds bounds = get_linebounds_space_consumed_by_text(screen_center, glm::vec2(1, 1), string_to_draw, text_draw_info);
	float text_width = get_horizontal_space_consumed_by_text(screen_center, glm::vec2(1, 1), string_to_draw, text_draw_info);
	float text_height = bounds.y_top - bounds.y_bot;
	Line camera_line;
	Line unscaled_text_line;

	camera_line.bot = camera.down;
	camera_line.top = camera.up;
	camera_line.length = camera.up - camera.down;
	camera_line.center = (camera.up + camera.down) / 2.0f;

	unscaled_text_line.bot = bounds.y_bot;
	unscaled_text_line.top = bounds.y_top;
	unscaled_text_line.length = bounds.y_top - bounds.y_bot;
	unscaled_text_line.center = (bounds.y_top + bounds.y_bot) / 2.0f;

	float max_scale_x_screenspace;
	float max_scale_y_screenspace;
	float x_dist = camera.right - camera.left;
	float y_dist = camera.up - camera.down;
	float x_scale = x_dist / text_width;
	float y_scale = y_dist / text_height;
	float scale = minf(x_scale, y_scale);

	float start_text_y_true_start;
	{
		float scaled_text_line_length = unscaled_text_line.length * scale;

		float vertical_space = camera_line.length - scaled_text_line_length;
		float half_vertical_space = vertical_space / 2.0f;

		float start_ratio = percentage_between(bounds.y_bot, bounds.y_top, 0);

		float start_text_bottom = camera.down + half_vertical_space;
		float start_text_top = start_text_bottom + scaled_text_line_length;
		start_text_y_true_start = lerp(start_text_bottom, start_text_top, start_ratio);
	}
	float start_text_x_true_start;
	{
		float true_width = text_width * scale;
		float x_space_left = x_dist - true_width;
		float x_space_left_half = x_space_left / 2.0f;
		start_text_x_true_start = camera.left + x_space_left_half;
	}
	//TODO: Finish this so the text is actually properly centered on the screen.
 //	float text_max_height = 

//	float x_center = (camera.right + camera.left) / 2.0f;
//	float x_start = screen_center.x - (text_width * x_scale) / 2.0f;
	draw_text_to_screen(glm::vec3(start_text_x_true_start, start_text_y_true_start, screen_center.z), glm::vec2(x_scale, x_scale), string_to_draw, text_draw_info);
}
