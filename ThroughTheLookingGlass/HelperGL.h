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
	int num_sprites_drawn;
	glm::vec4* atlas_mapper;
	glm::vec4* atlas_cpu;
	glm::mat4* final_cpu;
};

void fullsprite_generate(Shader shader, Memory* permanent_memory, GLuint vertices_VBO, GLuint vertices_EBO, glm::vec4* atlas_mapper,GamefullspriteDrawInfo* fullspriteDraw)
{
	fullspriteDraw->num_sprites_drawn = 0;
	fullspriteDraw->atlas_cpu = (glm::vec4*) memory_alloc(permanent_memory, MAX_NUM_FULL_SPRITES * sizeof(glm::vec4));
	fullspriteDraw->atlas_mapper = atlas_mapper;
	fullspriteDraw->final_cpu = (glm::mat4*) memory_alloc(permanent_memory, MAX_NUM_FULL_SPRITES * sizeof(glm::mat4));;
	shader_use(shader);
	//just use floor_atlas_mapper for fullsprite_atlas_mapper
	{
		glGenVertexArrays(1, &fullspriteDraw->fullsprite_VAO);
		std::cout << glGetError() << ":261" << std::endl;
		glBindVertexArray(fullspriteDraw->fullsprite_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO);

		int position = glGetAttribLocation(shader, "pos");
		glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(position);

		int texCoord = glGetAttribLocation(shader, "inputTexCoord");
		glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(texCoord);

		glGenBuffers(1, &fullspriteDraw->fullspriteMatrixBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, fullspriteDraw->fullspriteMatrixBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * MAX_NUM_FULL_SPRITES, NULL, GL_DYNAMIC_DRAW);

		int matrixOffset = 3;	//value hardcoded from fullsprite.vs
		glVertexAttribPointer(matrixOffset, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)0);
		std::cout << glGetError() << std::endl;
		std::cout << matrixOffset << "This is our matrix offset" << std::endl;
		glVertexAttribPointer(matrixOffset + 1, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(4 * sizeof(float)));

		glVertexAttribPointer(matrixOffset + 2, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(8 * sizeof(float)));
		glVertexAttribPointer(matrixOffset + 3, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)(12 * sizeof(float)));
		glEnableVertexAttribArray(matrixOffset);
		glEnableVertexAttribArray(matrixOffset + 1);
		glEnableVertexAttribArray(matrixOffset + 2);
		glEnableVertexAttribArray(matrixOffset + 3);
		glVertexAttribDivisor(matrixOffset, 1);
		glVertexAttribDivisor(matrixOffset + 1, 1);
		glVertexAttribDivisor(matrixOffset + 2, 1);
		glVertexAttribDivisor(matrixOffset + 3, 1);

		glGenBuffers(1, &fullspriteDraw->fullspriteAtlasBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, fullspriteDraw->fullspriteAtlasBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_NUM_FULL_SPRITES, NULL, GL_DYNAMIC_DRAW);

		int atlasOffset = glGetAttribLocation(shader, "atlasCoord");
		glVertexAttribPointer(atlasOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(atlasOffset);
		glVertexAttribDivisor(atlasOffset, 1);
	}
}
