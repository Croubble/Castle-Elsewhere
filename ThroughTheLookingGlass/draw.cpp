#include "Math.h"
#include "Shader.h"
#include <glad\glad.h>
#include "HelperGL.h"
#include "draw.h"
#include "GameState.h"
#include "Resource.h"
#include "sprite.h"

const int MAX_NUM_FULL_SPRITES = 100;

void fullsprite_generate(Shader shader, Memory* permanent_memory, GLuint vertices_VBO, GLuint vertices_EBO, glm::vec4* atlas_mapper, GamefullspriteDrawInfo* fullspriteDraw)
{
	fullspriteDraw->num_sprites_drawn = 0;
	fullspriteDraw->atlas_cpu = (glm::vec4*) memory_alloc(permanent_memory, MAX_NUM_FULL_SPRITES * sizeof(glm::vec4));
	fullspriteDraw->atlas_mapper = atlas_mapper;
	fullspriteDraw->final_cpu = (glm::mat4*) memory_alloc(permanent_memory, MAX_NUM_FULL_SPRITES * sizeof(glm::mat4));
	fullspriteDraw->color_cpu = (glm::vec4*) memory_alloc(permanent_memory, MAX_NUM_FULL_SPRITES * sizeof(glm::vec4));
	shader_use(shader);
	//just use floor_atlas_mapper for fullsprite_atlas_mapper
	{
		glGenVertexArrays(1, &fullspriteDraw->fullsprite_VAO); CHK
			std::cout << glGetError() << ":261" << std::endl;
		glBindVertexArray(fullspriteDraw->fullsprite_VAO); CHK

			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO); CHK
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO); CHK

			int position = glGetAttribLocation(shader, "pos"); CHK
			glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); CHK
			glEnableVertexAttribArray(position); CHK

			int texCoord = glGetAttribLocation(shader, "inputTexCoord"); CHK
			glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); CHK
			glEnableVertexAttribArray(texCoord); CHK

			glGenBuffers(1, &fullspriteDraw->fullspriteMatrixBuffer); CHK
			glBindBuffer(GL_ARRAY_BUFFER, fullspriteDraw->fullspriteMatrixBuffer); CHK
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * MAX_NUM_FULL_SPRITES, NULL, GL_DYNAMIC_DRAW); CHK

			int matrixOffset = 4;	//value hardcoded from fullsprite.vs
		glVertexAttribPointer(matrixOffset, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)0); CHK
			std::cout << glGetError() << std::endl;
		std::cout << matrixOffset << "This is our matrix offset" << std::endl;
		glVertexAttribPointer(matrixOffset + 1, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(4 * sizeof(float))); CHK

			glVertexAttribPointer(matrixOffset + 2, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(8 * sizeof(float))); CHK
			glVertexAttribPointer(matrixOffset + 3, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(12 * sizeof(float))); CHK
			glEnableVertexAttribArray(matrixOffset); CHK
			glEnableVertexAttribArray(matrixOffset + 1); CHK
			glEnableVertexAttribArray(matrixOffset + 2); CHK
			glEnableVertexAttribArray(matrixOffset + 3); CHK
			glVertexAttribDivisor(matrixOffset, 1); CHK
			glVertexAttribDivisor(matrixOffset + 1, 1); CHK
			glVertexAttribDivisor(matrixOffset + 2, 1); CHK
			glVertexAttribDivisor(matrixOffset + 3, 1); CHK

			glGenBuffers(1, &fullspriteDraw->fullspriteAtlasBuffer); CHK
			glBindBuffer(GL_ARRAY_BUFFER, fullspriteDraw->fullspriteAtlasBuffer); CHK
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_NUM_FULL_SPRITES, NULL, GL_DYNAMIC_DRAW); CHK

			int atlasOffset = glGetAttribLocation(shader, "atlasCoord");
		glVertexAttribPointer(atlasOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0)); CHK
			glEnableVertexAttribArray(atlasOffset); CHK
			glVertexAttribDivisor(atlasOffset, 1); CHK

			glGenBuffers(1, &fullspriteDraw->fullspriteColorBuffer); CHK
			glBindBuffer(GL_ARRAY_BUFFER, fullspriteDraw->fullspriteColorBuffer); CHK
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_NUM_FULL_SPRITES, NULL, GL_DYNAMIC_DRAW); CHK

			int colorOffset = 3;	//harded coded from ui.vs and fullsprite.vs, which are for now the same thing. 
		glVertexAttribPointer(colorOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0)); CHK
			glEnableVertexAttribArray(colorOffset); CHK
			glVertexAttribDivisor(colorOffset, 1); CHK

	}
}
void draw_black_box_over_screen(GameSpaceCamera screen, GamefullspriteDrawInfo* info)
{
	info->final_cpu[info->num_sprites_drawn] = glm::mat4(1.0f);
	info->final_cpu[info->num_sprites_drawn] = glm::translate(info->final_cpu[info->num_sprites_drawn], glm::vec3(screen.left, screen.down, 10));
	float height = screen.up - screen.down;
	float width = screen.right - screen.left;
	info->final_cpu[info->num_sprites_drawn] = glm::scale(info->final_cpu[info->num_sprites_drawn], glm::vec3(width, height, 1));
	info->atlas_cpu[info->num_sprites_drawn] = info->atlas_mapper[F_ZBLACK];
	info->num_sprites_drawn++;
}
void draw_outline_to_gamespace(AABB outline, SpriteWrite* info)
{
	info->matrix_cpu[info->num_draw] = glm::mat4(1.0f);
	info->matrix_cpu[info->num_draw] = glm::translate(info->matrix_cpu[info->num_draw], glm::vec3(outline.x, outline.y, 5));
	info->matrix_cpu[info->num_draw] = glm::scale(info->matrix_cpu[info->num_draw], glm::vec3(outline.w, outline.h, 1));
	info->atlas_cpu[info->num_draw] = info->atlas_mapper[F_ZBLACK];
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
		info->atlas_cpu[info->num_draw] = info->atlas_mapper[F_ZBLACK];
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
		info->atlas_cpu[info->num_draw] = info->atlas_mapper[F_ZBLACK];
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

void draw_stationary_piece_curses_to_gamespace(MovementAnimation* animation,
	DrawCurseAnimation* curse_animation,
	GameState** gamestates,
	IntPair* offsets,
	int number_of_gamestates,
	LayerDrawGPUData* info,
	int layer_index,
	float time_since_last_action)
{
	int i = layer_index;
	for (int z = 0; z < number_of_gamestates; z++)
	{
		GameState* gamestate = gamestates[z];
		IntPair offset = offsets[z];
		int num_elements_in_gamestate = gamestate->w * gamestate->h;
		for (int k = 0; k < num_elements_in_gamestate; k++)
		{
			int ele = gamestate->layers[i][k];
			CursedDirection curse_status = get_entities_cursed_direction(ele);
			if (curse_status != CursedDirection::NOTCURSED &&
				(!animation || (animation->start_offset[k].x == animation->end_offset[k].x && animation->start_offset[k].y == animation->end_offset[k].y)))
			{
				int curse_to_draw = resource_cursed_direction_to_piece_sprite(curse_status);
				info[i].atlas_cpu[info[i].total_drawn] = info[i].atlas_mapper[curse_to_draw];
				IntPair p = t2D(k, gamestate->w, gamestate->h);
				info[i].positions_cpu[info[i].total_drawn] = glm::vec3(offset.x + p.x, offset.y + p.y, Z_POSITION_STARTING_LAYER + i + 0.2f);
				info[i].movement_cpu[info[i].total_drawn] = glm::vec2(0, 0);
				if (curse_animation->flash[k])
				{
					float l = time_since_last_action / WAIT_BETWEEN_PLAYER_MOVE_REPEAT;
					l = minf(1.0f, l);
					float ln = l * NUM_CURSE_FLASHES * 2;
					float flash_value = (float)abs(fmod(ln, 2) - 1.0);
					glm::vec4 color = glm::mix(glm::vec4(0, 0, 0, 0), glm::vec4(1, 1, 1, 1), flash_value);
					info[i].color_cpu[info[i].total_drawn] = color;
				}
				else
				{
					info[i].color_cpu[info[i].total_drawn] = glm::vec4(1, 1, 1, 1);
				}
				info[i].total_drawn++;
			}

		}
	}
}

void draw_piece_curses_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index, DrawCurseAnimation* curse, float time_since_last_action)
{
	int i = layer_index;
	for (int z = 0; z < number_of_gamestates; z++)
	{
		GameState* gamestate = gamestates[z];
		IntPair offset = offsets[z];
		int num_elements_in_gamestate = gamestate->w * gamestate->h;
		for (int k = 0; k < num_elements_in_gamestate; k++)
		{
			int ele = gamestate->layers[i][k];
			CursedDirection curse_status = get_entities_cursed_direction(ele);
			if (curse_status != CursedDirection::NOTCURSED)
			{
				int curse_to_draw = resource_cursed_direction_to_piece_sprite(curse_status);
				info[i].atlas_cpu[info[i].total_drawn] = info[i].atlas_mapper[curse_to_draw];
				IntPair p = t2D(k, gamestate->w, gamestate->h);
				info[i].positions_cpu[info[i].total_drawn] = glm::vec3(offset.x + p.x, offset.y + p.y, Z_POSITION_STARTING_LAYER + i + 0.2f);
				info[i].movement_cpu[info[i].total_drawn] = glm::vec2(0, 0);
				if (curse && time_since_last_action < WAIT_BETWEEN_PLAYER_MOVE_REPEAT)
				{
					float l = time_since_last_action / WAIT_BETWEEN_PLAYER_MOVE_REPEAT;
					float ln = l * NUM_CURSE_FLASHES * 2;
					float flash_value = (float)abs(fmod(ln, 2) - 1);
					glm::vec4 color = glm::mix(glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 1, 1), flash_value);
					info[i].color_cpu[info[i].total_drawn] = color;
				}
				else
					info[i].color_cpu[info[i].total_drawn] = glm::vec4(1, 1, 1, 1);
				info[i].total_drawn++;
			}

		}
	}
}
bool draw_animation_to_gamespace(MovementAnimation* animation, LayerDrawGPUData* info_array, int layer_index, float time_since_last_action)
{
	float l = time_since_last_action / WAIT_BETWEEN_PLAYER_MOVE_REPEAT;

	l = maxf(0, l);
	l = minf(1, l);

	LayerDrawGPUData* info = &info_array[layer_index];
	int len = animation->num_elements;
	for (int i = 0; i < len; i++)
	{
		int ele = resource_layer_value_to_layer_sprite_value(animation->sprite_value[i], layer_index);
		if (ele == 0)
			continue;
		info->atlas_cpu[info->total_drawn] = info->atlas_mapper[ele];
		info->positions_cpu[info->total_drawn] = animation->start_position[i];
		info->movement_cpu[info->total_drawn] = animation->start_offset[i] * (1.0f - l) + animation->end_offset[i] * l;
		info->color_cpu[info->total_drawn] = glm::vec4(1, 1, 1, 1);
		info->total_drawn++;
	}

	return l >= 1;
}
void draw_curse_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info)
{
	int i = LN_PIECE;
	for (int z = 0; z < number_of_gamestates; z++)
	{
		GameState* gamestate = gamestates[z];
		IntPair offset = offsets[z];
		int num_elements_in_gamestate = gamestate->w * gamestate->h;
		for (int k = 0; k < num_elements_in_gamestate; k++)
		{
			int ele = gamestate->layers[i][k];
			CursedDirection curse_status = get_entities_cursed_direction(ele);
			if (curse_status != CursedDirection::NOTCURSED)
			{
				int curse_to_draw = resource_cursed_direction_to_piece_sprite(curse_status);
				info[i].atlas_cpu[info[i].total_drawn] = info[i].atlas_mapper[curse_to_draw];
				IntPair p = t2D(k, gamestate->w, gamestate->h);
				info[i].positions_cpu[info[i].total_drawn] = glm::vec3(offset.x + p.x, offset.y + p.y, Z_POSITION_STARTING_LAYER + i + 0.2f);
				info[i].movement_cpu[info[i].total_drawn] = glm::vec2(0, 0);
				info[i].color_cpu[info[i].total_drawn] = glm::vec4(1, 1, 1, 1);
				info[i].total_drawn++;
			}

		}
	}
}
void draw_layer_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index)
{
	int i = layer_index;
	{
		for (int z = 0; z < number_of_gamestates; z++)
		{
			GameState* gamestate = gamestates[z];
			IntPair offset = offsets[z];

			int num_elements_in_gamestate = gamestate->w * gamestate->h;
			for (int k = 0; k < num_elements_in_gamestate; k++)
			{
				int ele = gamestate->layers[i][k];
				int ele_image = resource_layer_value_to_layer_sprite_value(ele, i);
				info[i].atlas_cpu[info[i].total_drawn + k] = info[i].atlas_mapper[ele_image];
				info[i].movement_cpu[info[i].total_drawn + k] = glm::vec2(0, 0);
				IntPair p = t2D(k, gamestate->w, gamestate->h);
				info[i].positions_cpu[info[i].total_drawn + k] = glm::vec3(offset.x + p.x, offset.y + p.y, Z_POSITION_STARTING_LAYER + i);
				info[i].color_cpu[info[i].total_drawn + k] = glm::vec4(1, 1, 1, 1);
			}
			info[i].total_drawn += num_elements_in_gamestate;
		}
	}
}
void draw_layers_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info)
{
	for (int i = 0; i < GAME_NUM_LAYERS; i++)
	{
		draw_layer_to_gamespace(gamestates, offsets, number_of_gamestates, info, i);
	}
}

void draw_palette(IntPair palete_screen_start,
	GameSpaceCamera camera_game,
	ViewPortCamera camera_viewport,
	EditorUIState* ui_state,
	int palete_length,
	GamestateBrush* palete,
	LayerDrawGPUData* layer_draw)
{
	//parse palette data to gpu form
	{
		//determine what the gamespace position is from the screen position BOTTOM LEFT.
		glm::vec2 palete_true_start = math_screenspace_to_gamespace(palete_screen_start, camera_game, camera_viewport, ui_state->game_height_current);
		for (int i = 0; i < palete_length; i++)
		{
			if (palete[i].applyFloor)
			{
				LayerDrawGPUData* floor = &layer_draw[LN_FLOOR];
				int ele_image = resource_layer_value_to_layer_sprite_value(palete[i].floor, LN_FLOOR);
				floor->atlas_cpu[floor->total_drawn] = floor->atlas_mapper[ele_image];
				floor->positions_cpu[floor->total_drawn] = glm::vec3(palete_true_start.x + i, palete_true_start.y, 4);
				floor->movement_cpu[floor->total_drawn] = glm::vec2(0, 0);
				floor->color_cpu[floor->total_drawn] = glm::vec4(1, 1, 1, 1);
				floor->total_drawn++;
			}
			if (palete[i].applyPiece)
			{
				LayerDrawGPUData* piece = &layer_draw[LN_PIECE];
				int ele_image = resource_layer_value_to_layer_sprite_value(palete[i].piece, LN_PIECE);
				piece->atlas_cpu[piece->total_drawn] = piece->atlas_mapper[ele_image];
				piece->positions_cpu[piece->total_drawn] = glm::vec3(palete_true_start.x + i, palete_true_start.y, 5);
				piece->movement_cpu[piece->total_drawn] = glm::vec2(0, 0);
				piece->color_cpu[piece->total_drawn] = glm::vec4(1, 1, 1, 1);
				piece->total_drawn++;
			}
		}
	}
}