#pragma once

#include "Constants.h"
#include <glad\glad.h>
#include "Memory.h"
#include "glm/glm.hpp"

void gl_generate_and_bind_VAO(GLuint* location);

void gl_setup_defaults();
void gl_setup_int_vertex(GLuint* buffer, GLuint gl_attrib_location, int vector_size, int max_buffer_length);
void gl_setup_float_vertex(GLuint* buffer, GLuint gl_attrib_location, int vector_size, int max_buffer_length);
#define CHK do {gl_check_err(__FILE__,__LINE__);} while(0);

void gl_check_err(const char* filename, int line);

