#pragma once
#include <glm\ext\vector_int2.hpp>

struct TTF_Character
{
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int advance;
};