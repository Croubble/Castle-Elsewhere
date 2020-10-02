#pragma once

#include "Constants.h"
#include <glad\glad.h>
#include "Memory.h"
#include "glm/glm.hpp"

const int MAX_NUM_FULL_SPRITES = 100;

void gl_generate_and_bind_VAO(GLuint* location)
{
	glGenVertexArrays(1, location);
	glBindVertexArray(*location);
}

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


#define CHK gl_check_err(__FILE__,__LINE__);



void gl_check_err(const char* filename, int line)
{
	int error_check = glGetError();
	if (error_check != 0)
		std::cout << "we have produced an openGL error on line" << line << " , in file " << filename << "better check it out." << error_check <<  std::endl;
}
void fullsprite_generate(Shader shader, Memory* permanent_memory, GLuint vertices_VBO, GLuint vertices_EBO, glm::vec4* atlas_mapper,GamefullspriteDrawInfo* fullspriteDraw)
{
	fullspriteDraw->num_sprites_drawn = 0;
	fullspriteDraw->atlas_cpu = (glm::vec4*) memory_alloc(permanent_memory, MAX_NUM_FULL_SPRITES * sizeof(glm::vec4));
	fullspriteDraw->atlas_mapper = atlas_mapper;
	fullspriteDraw->final_cpu = (glm::mat4*) memory_alloc(permanent_memory, MAX_NUM_FULL_SPRITES * sizeof(glm::mat4));;
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
