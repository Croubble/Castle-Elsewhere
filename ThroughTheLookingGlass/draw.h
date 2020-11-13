#pragma once
#include "Animation.h"
#include "Editor.h"
#include "Click.h"
#include "sprite.h"
#include "textureAssets.h"
struct LayerDrawGPUData
{
	int total_drawn;
	GLuint texture;

	GLuint VAO;

	GLuint positions_VBO;
	glm::vec3* positions_cpu;

	GLuint atlas_VBO;
	glm::vec4* atlas_cpu;

	GLuint movement_VBO;
	glm::vec2* movement_cpu;

	GLuint color_VBO;
	glm::vec4* color_cpu;

	glm::vec4* atlas_mapper;
};

struct GamefullspriteDrawInfo {
	GLuint fullsprite_VAO;
	GLuint fullspriteMatrixBuffer;
	GLuint fullspriteAtlasBuffer;
	GLuint fullspriteColorBuffer;
	int num_sprites_drawn;
	glm::vec4* atlas_mapper;
	glm::vec4* atlas_cpu;
	glm::mat4* final_cpu;
	glm::vec4* color_cpu;
};

/************************************************************/
/**********draw functions************************************/
void fullsprite_generate(Shader shader, Memory* permanent_memory, GLuint vertices_VBO, GLuint vertices_EBO, glm::vec4* atlas_mapper, GamefullspriteDrawInfo* fullspriteDraw);
void draw_curse_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info);
void draw_stationary_piece_curses_to_gamespace(MovementAnimation* animation, DrawCurseAnimation* curse, GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index, float time_since_last_action);
void draw_piece_curses_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index, DrawCurseAnimation* curse, float time_since_last_action);
bool draw_animation_to_gamespace(MovementAnimation* animation, LayerDrawGPUData* info_array, int layer_index, float time_since_last_action);
void draw_layer_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info, int layer_index);
void draw_layers_to_gamespace(GameState** gamestates, IntPair* offsets, int number_of_gamestates, LayerDrawGPUData* info);
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
	LayerDrawGPUData* layer_draw);
void draw_black_box_over_screen(GameSpaceCamera screen, SpriteWrite* info);
void draw_outline_to_gamespace(AABB outline, SpriteWrite* info);
void draw_button_to_gamespace(GameSpaceCamera draw_area, SpriteWrite* ui_draw, glm::vec4 color = glm::vec4(1,1,1,1));
void draw_ui_to_gamespace(GameSpaceCamera draw_area, int index, SpriteWrite* draw_info, glm::vec4 color = glm::vec4(1, 1, 1, 1));

