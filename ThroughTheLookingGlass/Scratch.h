#pragma once

#include "Math.h"
#include "Shader.h"
typedef unsigned int uint;
//get the gamestate we want to draw from the world, the position we want to draw it.
//detect whether we can draw movement,
//(TO DELETE: detect whether we can draw cursed animation -> this might happen even if we don't move)

struct SpriteOut {
	Shader shader;

	uint VAO;
	uint matrix_buf;
	uint atlas_buf;
	uint color_buf;

	uint max_draw;
	uint num_draw;
	glm::vec4 atlas_mapper;
	glm::vec4 atlas_cpu;
	glm::vec4 final_cpu;
	glm::vec4 color_cpu;
};
struct DrawData
{
	//floor layer
	//piece layer
	//symbol layer
	//Shapes layer
};

void sprite_make(Shader shader, Memory* permanent_memory, GLuint vertices_VBO, GLuint vertices_EBO, glm::vec4* atlas_mapper, SpriteOut* spriteDraw);
//IDEA: Just because we don't put in an action, doesn't mean we don't want to reset our draw_timer and our animation_info
/*
void draw_algorithm(DrawOutput* output, GameState* state, IntPair draw__offset_position, ProcessedJournal* journal)
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
