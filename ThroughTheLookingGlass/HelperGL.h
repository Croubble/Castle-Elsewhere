#pragma once

#include "Constants.h"
#include <glad\glad.h>

void gl_generate_and_bind_VAO(GLuint* location)
{
	glGenVertexArrays(1, location);
	glBindVertexArray(*location);
}
