#include "HelperGL.h"

void gl_generate_and_bind_VAO(GLuint* location)
{
	glGenVertexArrays(1, location);
	glBindVertexArray(*location);
}

void gl_setup_defaults()
{

}

void gl_setup_int_vertex(GLuint* buffer, GLuint gl_attrib_location, int vector_size, int max_buffer_length)
{
	if (vector_size <= 0 || vector_size > 4)
	{
		crash_err("incorrectly called gl_setup_vertex with a vector size of " + vector_size);
	}
	glGenBuffers(1, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, *buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(int) * vector_size * max_buffer_length, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(gl_attrib_location, vector_size, GL_FLOAT, false, vector_size * sizeof(int), (void*)0);
	glEnableVertexAttribArray(gl_attrib_location);
	glVertexAttribDivisor(gl_attrib_location, 1);
}

void gl_setup_float_vertex(GLuint* buffer, GLuint gl_attrib_location, int vector_size, int max_buffer_length)
{
	if (vector_size <= 0 || vector_size > 4)
	{
		crash_err("incorrectly called gl_setup_vertex with a vector size of " + vector_size);
	}
	glGenBuffers(1, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, *buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vector_size * max_buffer_length, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(gl_attrib_location, vector_size, GL_FLOAT, false, vector_size * sizeof(float), (void*)0);
	glEnableVertexAttribArray(gl_attrib_location);
	glVertexAttribDivisor(gl_attrib_location, 1);
}

void gl_check_err(const char* filename, int line)
{
	int error_check = glGetError();
	if (error_check != 0)
		std::cout << "we have produced an openGL error on line" << line << " , in file " << filename << "better check it out." << error_check << std::endl;
}
