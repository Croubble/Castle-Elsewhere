#pragma once

#pragma once

#include "Math.h"
#include "Shader.h"
#include "HelperGL.h"
#include "TextDraw.h"


typedef unsigned int uint;
//get the gamestate we want to draw from the world, the position we want to draw it.
//detect whether we can draw movement,
//(TO DELETE: detect whether we can draw cursed animation -> this might happen even if we don't move)

struct SpriteWrite {
	Shader shader;
	uint atlas_texture;

	uint VAO;
	uint matrix_buf;
	uint atlas_buf;
	uint color_buf;

	uint max_draw;
	uint num_draw;
	glm::vec4* atlas_mapper;
	glm::vec4* atlas_cpu;
	glm::mat4* matrix_cpu;
	glm::vec4* color_cpu;
};
struct DrawData
{
	SpriteWrite* floor;
	SpriteWrite* piece;
	SpriteWrite* symbol;
	SpriteWrite* ui;
	TextWrite* text;
};

SpriteWrite* sprite_write_make(GLuint atlas_texture, Shader shader, Memory* permanent_memory, int max_draw, GLuint vertices_VBO, GLuint vertices_EBO, glm::vec4* atlas_mapper);


void sprite_write_out(SpriteWrite* out, glm::mat4 camera);
//IDEA: Just because we don't put in an action, doesn't mean we don't want to reset our draw_timer and our animation_info
/*
void draw_algorithm(DrawOutput* out->ut, GameState* state, IntPair draw__offset_position, ProcessedJournal* journal)
{
	//draw floor
	if(!journal)
	{
		//static draw algorithm
		//draw pieces(
		//draw crate data
	}
	if(journal)
	{
		//for each piece, if its moving, draw it lerping from old_position to new_position using time_since_last_action.
		//for each crate data, draw the symbols, lerping from old position to new_position using time_since_last_action.
	}
}
*/
