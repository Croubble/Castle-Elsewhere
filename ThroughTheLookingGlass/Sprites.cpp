#include "Sprites.h"
#include <glm\ext\matrix_transform.hpp>

void sprites_fullsprite_to_gpu_data(FullSpriteArray* sprites, glm::vec4* index_mapper, glm::vec4* atlas_output, glm::mat4* matrix_output)
{
	int length = sprites->length;
	for (int i = 0; i < length; i++)
	{
		atlas_output[i] = index_mapper[sprites->atlasIndex[i]];
	}
	glm::vec3 translate_before_rotate_start = glm::vec3(-0.5f, -0.5f, 0);
	glm::vec3 translate_after_rotate_end = glm::vec3(0.5f, 0.5f, 0);
	glm::vec3 rotate_vec3 = glm::vec3(0, 0, 1);
	for (int i = 0; i < length; i++)
	{
		//setup identity matrix.
		matrix_output[i] = glm::mat4(1.0f);

		//step 3: translate.
		glm::translate(matrix_output[i], glm::vec3(sprites->positionWithScale[i].x,sprites->positionWithScale[i].y,0));

		//step 2: scale.
		glm::scale(matrix_output[i], glm::vec3(sprites->positionWithScale[i].z, sprites->positionWithScale[i].w, 1));

		//step 1: rotate.
		glm::translate(matrix_output[i], translate_before_rotate_start);
		glm::rotate(matrix_output[i], sprites->rotation[i], rotate_vec3);
		glm::translate(matrix_output[i], translate_after_rotate_end);
	}
}
void sprites_boxsprite_to_gpu_data(BoxSprites* sprites, glm::vec4* index_mapper, glm::vec4* atlas_output, glm::vec2* position_output)
{
	int length = sprites->length;
	for (int i = 0; i < length; i++)
	{
		atlas_output[i] = index_mapper[sprites->atlasIndex[i]];
	}
	for (int i = 0; i < length; i++)
	{
		position_output[i] = sprites->position[i];
	}
}
