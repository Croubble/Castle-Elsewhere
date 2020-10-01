#pragma once
#include "PreludeIncludes.h"
#include <glm\ext\vector_float4.hpp>
#include <glm\ext\vector_float3.hpp>

struct TextureAtlasMaps
{
	unsigned int* layer_texture_atlas;
	glm::vec4** layer_element_texture_position;
};

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


