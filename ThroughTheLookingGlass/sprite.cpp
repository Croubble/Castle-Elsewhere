#include "sprite.h"

SpriteWrite* sprite_write_make(GLuint atlas_texture, Shader shader, Memory* permanent_memory, int max_draw, GLuint vertices_VBO, GLuint vertices_EBO, glm::vec4* atlas_mapper) 
{
	SpriteWrite* result = (SpriteWrite*)memory_alloc(permanent_memory, sizeof(SpriteWrite));
	result->shader = shader;
	result->atlas_texture = atlas_texture;
	result->max_draw = 0;
	result->num_draw = 0;
	result->atlas_cpu = (glm::vec4*) memory_alloc(permanent_memory, max_draw * sizeof(glm::vec4));
	result->atlas_mapper = atlas_mapper;
	result->matrix_cpu = (glm::mat4*) memory_alloc(permanent_memory, max_draw * sizeof(glm::mat4));
	result->color_cpu = (glm::vec4*) memory_alloc(permanent_memory, max_draw * sizeof(glm::vec4));

	shader_use(shader);
	//setup buffers on the gpu.
	{
		glGenVertexArrays(1, &result->VAO);
		glBindVertexArray(result->VAO);

		glGenVertexArrays(1, &result->VAO); CHK
			glBindVertexArray(result->VAO); CHK

			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO); CHK
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO); CHK

			int position = glGetAttribLocation(shader, "pos"); CHK
			glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); CHK
			glEnableVertexAttribArray(position); CHK

			int texCoord = glGetAttribLocation(shader, "inputTexCoord"); CHK
			glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); CHK
			glEnableVertexAttribArray(texCoord); CHK

			glGenBuffers(1, &result->matrix_buf); CHK
			glBindBuffer(GL_ARRAY_BUFFER, result->matrix_buf); CHK
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * max_draw, NULL, GL_DYNAMIC_DRAW); CHK

			int matrixOffset = 4;	//value hardcoded from fullsprite.vs
		glVertexAttribPointer(matrixOffset, 4, GL_FLOAT, false, 16 * sizeof(float), (void*)0); CHK
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

			glGenBuffers(1, &result->atlas_buf); CHK
			glBindBuffer(GL_ARRAY_BUFFER, result->atlas_buf); CHK
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * max_draw, NULL, GL_DYNAMIC_DRAW); CHK

			int atlasOffset = glGetAttribLocation(shader, "atlasCoord");
		glVertexAttribPointer(atlasOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0)); CHK
			glEnableVertexAttribArray(atlasOffset); CHK
			glVertexAttribDivisor(atlasOffset, 1); CHK

			glGenBuffers(1, &result->color_buf); CHK
			glBindBuffer(GL_ARRAY_BUFFER, result->color_buf); CHK
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * max_draw, NULL, GL_DYNAMIC_DRAW); CHK

			int colorOffset = 3;	//harded coded from ui.vs and fullsprite.vs, which are for now the same thing. 
		glVertexAttribPointer(colorOffset, 4, GL_FLOAT, false, 4 * sizeof(float), (void*)(0)); CHK
			glEnableVertexAttribArray(colorOffset); CHK
			glVertexAttribDivisor(colorOffset, 1); CHK
			glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO); CHK
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO); CHK
	}
	return result;
}

void sprite_write_out(SpriteWrite* out, glm::mat4 camera)
{
	shader_set_uniform_mat4(out->shader, "viewProjectionMatrix", camera);
	//send it on over to gpu!
	glUseProgram(out->shader);
	glBindVertexArray(out->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, out->atlas_buf);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * out->num_draw, out->atlas_cpu);

	glBindBuffer(GL_ARRAY_BUFFER, out->matrix_buf);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * out->num_draw, out->matrix_cpu);

	glBindBuffer(GL_ARRAY_BUFFER, out->color_buf);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * out->num_draw, out->color_cpu);

	glBindTexture(GL_TEXTURE_2D, out->atlas_texture);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, out->num_draw);
}
