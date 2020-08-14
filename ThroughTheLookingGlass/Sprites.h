#pragma once
#include <glm/glm.hpp>
#include "Math.h"
struct BoxSprites {
	int length;
	int* atlasIndex;
	glm::vec2* position;
};

struct FullSpriteArray
{
	int length;
	int* atlasIndex;
	glm::vec4* positionWithScale;
	float* rotation;
};

void sprites_fullsprite_to_aabb_and_gpu_data(FullSpriteArray* sprites, AABB* aabb_output, glm::vec4* atlas_output, glm::mat4* matrix_output);
void sprites_boxsprite_to_aabb_and_gpu_data(BoxSprites* sprites, AABB* aabb_output, glm::vec4* atlas_output, glm::mat4* matrix_output);
