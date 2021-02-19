#include "Math.h"
#include "Shader.h"
#include <glad\glad.h>
#include "HelperGL.h"
#include "draw.h"
#include "GameState.h"
#include "Resource.h"
#include "sprite.h"

const int MAX_NUM_FULL_SPRITES = 100;

void draw_black_box_over_screen(GameSpaceCamera screen, SpriteWrite* info)
{
	info->matrix_cpu[info->num_draw] = glm::mat4(1.0f);
	info->matrix_cpu[info->num_draw] = glm::translate(info->matrix_cpu[info->num_draw], glm::vec3(screen.left, screen.down, 10));
	float height = screen.up - screen.down;
	float width = screen.right - screen.left;
	info->matrix_cpu[info->num_draw] = glm::scale(info->matrix_cpu[info->num_draw], glm::vec3(width, height, 1));
	info->atlas_cpu[info->num_draw] = info->atlas_mapper[resource_floor_to_sprite(F_ZBLACK)];
	info->num_draw++;
}
void draw_outline_to_gamespace(AABB outline, SpriteWrite* info)
{
	info->matrix_cpu[info->num_draw] = glm::mat4(1.0f);
	info->matrix_cpu[info->num_draw] = glm::translate(info->matrix_cpu[info->num_draw], glm::vec3(outline.x, outline.y, 5));
	info->matrix_cpu[info->num_draw] = glm::scale(info->matrix_cpu[info->num_draw], glm::vec3(outline.w, outline.h, 1));
	info->atlas_cpu[info->num_draw] = info->atlas_mapper[resource_floor_to_sprite(F_ZBLACK)];
	info->num_draw++;
}
void draw_gamestates_outlines_to_gamespace(GameState** gamestates, IntPair* offsets, int length_function_input, SpriteWrite* info, int skip_index)
{
	for (int z = 0; z < skip_index; z++)
	{
		int w = gamestates[z]->w;
		int h = gamestates[z]->h;
		IntPair startPos = offsets[z];
		glm::mat4 final = glm::mat4(1.0f);
		final = glm::translate(final, glm::vec3(startPos.x - OUTLINE_DRAW_SIZE, startPos.y - OUTLINE_DRAW_SIZE, 0));
		final = glm::scale(final, glm::vec3(w + OUTLINE_DRAW_SIZE * 2, h + OUTLINE_DRAW_SIZE * 2, 1));
		info->matrix_cpu[info->num_draw] = final;
		info->atlas_cpu[info->num_draw] = info->atlas_mapper[resource_floor_to_sprite(F_ZBLACK)];
		info->num_draw++;
	}
	for (int z = skip_index + 1; z < length_function_input; z++)
	{
		int w = gamestates[z]->w;
		int h = gamestates[z]->h;
		IntPair startPos = offsets[z];
		glm::mat4 final = glm::mat4(1.0f);
		final = glm::translate(final, glm::vec3(startPos.x - OUTLINE_DRAW_SIZE, startPos.y - OUTLINE_DRAW_SIZE, 0));
		final = glm::scale(final, glm::vec3(w + OUTLINE_DRAW_SIZE * 2, h + OUTLINE_DRAW_SIZE * 2, 1));
		info->matrix_cpu[info->num_draw] = final;
		info->atlas_cpu[info->num_draw] = info->atlas_mapper[resource_floor_to_sprite(F_ZBLACK)];
		info->num_draw++;
	}
}
void draw_ui_to_gamespace(GameSpaceCamera draw_area, int index, SpriteWrite* draw_info, glm::vec4 color)
{
	int current_draw = draw_info->num_draw;
	draw_info->atlas_cpu[current_draw] = draw_info->atlas_mapper[index];
	draw_info->matrix_cpu[current_draw] = glm::mat4(1.0f);
	draw_info->matrix_cpu[current_draw] = glm::translate(draw_info->matrix_cpu[current_draw], glm::vec3(draw_area.left, draw_area.down, 1));
	float width = draw_area.right - draw_area.left;
	float height = draw_area.up - draw_area.down;
	draw_info->matrix_cpu[current_draw] = glm::scale(draw_info->matrix_cpu[current_draw], glm::vec3(width, height, 1));
	draw_info->color_cpu[current_draw] = color;
	draw_info->num_draw++;

};
void draw_button_to_gamespace(GameSpaceCamera draw_area, SpriteWrite * ui_draw, glm::vec4 color)
{
	//draw the left half.
	float width = draw_area.right - draw_area.left;
	float height = draw_area.up - draw_area.down;
	if (height >= width)
	{
		crash_err("we can't draw a button if it isn't at least 1x1, the draw won't work");

	}
	//draw left.
	{
		GameSpaceCamera left_draw = draw_area;
		left_draw.right = left_draw.left + 0.5f * height;
		draw_ui_to_gamespace(left_draw,textureAssets::UI::ButtonLeftHalf, ui_draw, color);
	}
	//(maybe) draw the middle half.
	{
		GameSpaceCamera middle_draw = draw_area;
		middle_draw.left += 0.5f * height;
		middle_draw.right -= 0.5f * height;
		draw_ui_to_gamespace(middle_draw, textureAssets::UI::ButtonCenter, ui_draw, color);
	}
	if (height != width)
	{
		GameSpaceCamera right_draw = draw_area;
		right_draw.left = right_draw.right - 0.5f * height;
		draw_ui_to_gamespace(right_draw,textureAssets::UI::ButtonRightHalf ,ui_draw, color);
	}
	//draw the right half.
}

void mask_sprite_using_window(glm::vec4* draw_square, glm::vec4* tex_coord, glm::vec4 window, glm::vec2 tex_movement)
{
	glm::vec4 next_draw_square;
	//build a new draw square.
	{
		next_draw_square.x = maxf(draw_square->x, window.x);
		next_draw_square.z = minf(draw_square->z, window.z);
		next_draw_square.y = maxf(draw_square->y, window.y);
		next_draw_square.w = minf(draw_square->w, window.w);
	}
	//determine what % of the draw square got reduced in L/R/U/D, and then reduce the window by that amount.
	auto determine_atlas_change = [](float pos, float window, float length_square) -> float 
	{
		float percentage_change = (pos - window) / length_square;
		return percentage_change;
	};
	glm::vec4 next_tex_coord;
	{
		float width = draw_square->z - draw_square->x;
		float height = draw_square->w - draw_square->y;

		float left = clampf(0, 1, 0 - tex_movement.x / width);
		float right = clampf(0, 1, 1 - tex_movement.x / width);
		float down = clampf(0, 1, 0 - tex_movement.y / height);
		float up = clampf(0, 1, 1 - tex_movement.y / height);
		
		next_tex_coord.x = lerp(tex_coord->x, tex_coord->z, left);
		next_tex_coord.z = lerp(tex_coord->x, tex_coord->z, right);
		next_tex_coord.y = lerp(tex_coord->y, tex_coord->w, down);
		next_tex_coord.w = lerp(tex_coord->y, tex_coord->w, up);
	}
	//output those new values to our input data.
	*draw_square = next_draw_square;
	*tex_coord = next_tex_coord;
}
bool draw_crate_traits_to_gamespace(int* pieces, PieceData* piece_data, MovementAnimation* animation, SpriteWrite* info, int layer_index, float time_since_last_action)
{
	//generate 
	return false;
}
bool draw_animation_to_gamespace(MovementAnimation* animation, SpriteWrite * info, int layer_index, float time_since_last_action)
{
	float l = time_since_last_action / WAIT_BETWEEN_PLAYER_MOVE_REPEAT;
	l = l * 0.1f;
	l = maxf(0, l);
	l = minf(1, l);

	int len = animation->num_elements;
	for (int i = 0; i < len; i++)
	{
		if (animation->sprite_value[i] == 0)
			continue;

		int ele_img = resource_layer_value_to_layer_sprite_value(animation->sprite_value[i], layer_index);
		glm::vec4 temp_atlas_pos = info->atlas_mapper[ele_img];
		float x_move = animation->start_offset[i].x * (1.0f - l) + animation->end_offset[i].x * l;
		float y_move = animation->start_offset[i].y * (1.0f - l) + animation->end_offset[i].y * l;
		
		//calculate the position we will be drawing too.
		/*
		float x_min = animation->position[i].x;
		float x_max = animation->position[i].x + 1;
		float x_pos = x_min + x_move;
		float y_min = animation->position[i].y;
		float y_max = animation->position[i].y + 1;
		float y_pos = y_min + y_move;
		float xl = clampf(x_min, x_max, x_pos);
		float xr = clampf(x_min, x_max, x_pos + 1);
		float yb = clampf(y_min, y_max, y_pos);
		float yt = clampf(y_min, y_max, y_pos + 1);
		float x_width = xr - xl;
		float y_height = yt - yb;

		//figure out the amount of distance we lost 
		glm::vec4 temp_atlas_pos = info->atlas_mapper[ele_img];
		glm::vec4 atlas_pos;
		{
			float wx = animation->position[i].x;	//window_x
			float wy = animation->position[i].y;  //window_y
			float px = x_pos;
			float py = y_pos;
			float x_left = maxf(0,wx - px);
			float x_right = minf(1,1 + (wx - px));
			float y_bot = maxf(0, wy - py);
			float y_top = minf(1, 1 + (wy - py));

			atlas_pos.x = lerpf(temp_atlas_pos.x, temp_atlas_pos.z, x_left);
			atlas_pos.z = lerpf(temp_atlas_pos.x, temp_atlas_pos.z, x_right);
			atlas_pos.y = lerpf(temp_atlas_pos.y, temp_atlas_pos.w, y_bot);
			atlas_pos.w = lerpf(temp_atlas_pos.y, temp_atlas_pos.w, y_top);
		}
		*/
		float x_start = animation->position[i].x + x_move;
		float y_start = animation->position[i].y + y_move;
		glm::vec4 square = glm::vec4(x_start,y_start, x_start + 1, y_start + 1);
		float window_start_x = animation->position[i].x;
		float window_start_y = animation->position[i].y;
		glm::vec4 window = glm::vec4(window_start_x, window_start_y, window_start_x + 1, window_start_y + 1);
		mask_sprite_using_window(&square, &temp_atlas_pos, window,glm::vec2(x_move,y_move));

		glm::mat4 matrix;
		{
			float x_width = (square.z - square.x);
			float y_height = (square.w - square.y);
			glm::vec3 final_translate = glm::vec3(square.x, square.y, animation->position[i].z);
			glm::vec3 final_scale = glm::vec3(x_width, y_height, 1);
			matrix = math_translated_scaled_matrix(final_translate, final_scale);
		}

		info->atlas_cpu[info->num_draw] = temp_atlas_pos;
		info->matrix_cpu[info->num_draw] = matrix;
//		info->matrix_cpu[info->num_draw] = math_translated_scaled_matrix(glm::vec3(xl,yb,animation->position[i].z),glm::vec3(x_width,y_height,1));
		info->color_cpu[info->num_draw] = glm::vec4(1, 1, 1, 1);
		info->num_draw++;
	}

	return l >= 1;
}
void draw_layer_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, SpriteWrite* info, int layer_index)
{
	int i = layer_index;
	{
		for (int z = 0; z < number_of_gamestates; z++)
		{
			GameState* gamestate = gamestates[z];
			IntPair offset = offsets[z];
			int* current_layer = gamestate_get_layer(gamestate,layer_index);
			int num_elements_in_gamestate = gamestate->w * gamestate->h;
			for (int k = 0; k < num_elements_in_gamestate; k++)
			{
				int ele = current_layer[k];
				int ele_image = resource_layer_value_to_layer_sprite_value(ele, i);
				info->atlas_cpu[info->num_draw + k] = info->atlas_mapper[ele_image];
				IntPair p = t2D(k, gamestate->w, gamestate->h);
				glm::vec3 translate = glm::vec3(offset.x + p.x, offset.y + p.y, Z_POSITION_STARTING_LAYER + i);
				info->matrix_cpu[info->num_draw + k] = math_translated_matrix(translate);
				info->color_cpu[info->num_draw+ k] = glm::vec4(1, 1, 1, 1);
			}
			info->num_draw += num_elements_in_gamestate;
		}
	}
}
void draw_sprite(int atlas_ele, glm::mat4 position, SpriteWrite* write_to)
{
	glm::vec4 atlas_coords = write_to->atlas_mapper[atlas_ele];
	int d = write_to->num_draw;
	write_to->atlas_cpu[d] = atlas_coords;
	write_to->matrix_cpu[d] = position;
	write_to->color_cpu[d] = glm::vec4(1, 1, 1, 1);
	write_to->num_draw++;
}



void draw_piece_data(PieceData piece_data, glm::vec2 draw_start, SpriteWrite* symbols)
{
	//determine whether we need 1 space, 4 spaces, or 9 spaces to draw the crate.
	int length = CP_COUNT;
	int total = 0;
	for (int i = 0; i < length; i++)
		total += piece_data.powers[i];
	
	int num_drawn = 0;//draw number.
	int width = (int) ceil(sqrt(total));

	for (int j = 0; j < length; j++)
	{
		if (piece_data.powers[j])
		{
			//draw position
			int to_draw = piecedata_to_symbol((CratePower) j);
			glm::vec3 draw_position = glm::vec3(num_drawn % width, num_drawn / width, 5);
			glm::vec3 scale = glm::vec3(1.0 / (float)width, 1.0 / (float)width, 1);
			scale.x -= 0.10f;
			scale.y -= 0.10f;
			draw_position.x /= (float)width;
			draw_position.y /= (float)width;
			draw_position.x += draw_start.x;
			draw_position.y += draw_start.y;
			draw_position.x += 0.05f;
			draw_position.y += 0.05f;
			num_drawn++;
			glm::mat4 matrix_drawn = math_translated_scaled_matrix(draw_position, scale);
			draw_sprite(to_draw, matrix_drawn, symbols);
		}
	}

};

void draw_gamestate_pieces_animated(GameState* gamestate, MovementAnimation* animation, IntPair offset, AllWrite* info)
{

}
void draw_gamestate_pieces(GameState* gamestate, IntPair offset, AllWrite* info)
{
	GameState* next = gamestate;
		for (int i = 0; i < next->w * next->h; i++)
			if (next->piece[i] == P_CRATE)
			{
				PieceData piece_data = next->piece_data[i];
				IntPair state_offset = offset;
				IntPair grid_offset = t2D(i, next->w, next->h);
				glm::vec2 draw_offset = glm::vec2(state_offset.x + grid_offset.x, state_offset.y + grid_offset.y);
				draw_piece_data(piece_data, draw_offset, info->symbol);
			}

}
void draw_gamespace_animated(GameState** gamestates, Animations** maybe_animations, IntPair* offsets, int number_of_gamestates, AllWrite* info, float time_since_action)
{
	draw_layer_to_gamespace(gamestates, offsets, number_of_gamestates, info->floor, 0);
	for (int z = 0; z < number_of_gamestates; z++)
	{

		GameState* next = gamestates[z];
		IntPair offset = offsets[z];
		if (!maybe_animations[z])
		{
			draw_layer_to_gamespace(&gamestates[z], &offsets[z], 1, info->piece, 1);
			draw_gamestate_pieces(next, offset, info);
		}
		else
		{
			draw_animation_to_gamespace(maybe_animations[z]->maybe_movement_animation, info->piece, 1, time_since_action);
			//draw_gamestate_pieces(next, offset, info);
		}
	}
}
void draw_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, AllWrite* info)
{
	{
		draw_layer_to_gamespace(gamestates, offsets, number_of_gamestates, info->floor, 0);
		draw_layer_to_gamespace(gamestates, offsets, number_of_gamestates, info->piece, 1);
		//draw crate symbols.
		for(int z = 0; z < number_of_gamestates;z++)
		{
			GameState* next = gamestates[z];
			IntPair offset = offsets[z];
			draw_gamestate_pieces(next, offset, info);
		}
	}
}

void draw_palette(IntPair palete_screen_start,
	GameSpaceCamera camera_game,
	ViewPortCamera camera_viewport,
	EditorUIState* ui_state,
	int palete_length,
	GamestateBrush* palete,
	AllWrite* layer_draw)
{
	//parse palette piece_data to gpu form
	{
		//determine what the gamespace position is from the screen position BOTTOM LEFT.
		glm::vec2 palete_true_start = math_screenspace_to_gamespace(palete_screen_start, camera_game, camera_viewport, ui_state->game_height_current);
		for (int i = 0; i < palete_length; i++)
		{
			if (palete[i].applyFloor)
			{
				SpriteWrite* floor = layer_draw->floor;
				int ele_image = resource_layer_value_to_layer_sprite_value(palete[i].floor, LN_FLOOR);
				floor->atlas_cpu[floor->num_draw] = floor->atlas_mapper[ele_image];
				floor->matrix_cpu[floor->num_draw] = math_translated_matrix(glm::vec3(palete_true_start.x + i, palete_true_start.y,4));
				floor->color_cpu[floor->num_draw] = glm::vec4(1, 1, 1, 1);
				floor->num_draw++;
			}
			if (palete[i].applyPiece)
			{
				SpriteWrite* piece = layer_draw->piece;
				int ele_image = resource_layer_value_to_layer_sprite_value(palete[i].piece, LN_PIECE);
				piece->atlas_cpu[piece->num_draw] = piece->atlas_mapper[ele_image];
				piece->matrix_cpu[piece->num_draw] = math_translated_matrix(glm::vec3(palete_true_start.x + i, palete_true_start.y, 5));
				piece->color_cpu[piece->num_draw] = glm::vec4(1, 1, 1, 1);
				piece->num_draw++;
			}
		}
	}
}