#include <string>

#include "PreludeIncludes.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Resource.h"
#include <iostream>

typedef GLuint Shader;

bool test_strings_start_equals_string_internal(std::string start_test, std::string string)
{
	if (start_test.length() > string.length())
		return false;
	for (int i = 0; i < start_test.length(); i++)
		if (start_test[i] != string[i])
			return false;
	return true;
}

std::string convert_core_to_es_internal(std::string input)
{
	std::string to_delete = "#version 300 core";
	std::string to_replace = "#version 300 es";
	int len = to_delete.length() + 2;
	std::string result = to_replace + input.substr(17,std::string::npos);
	return result;
}

std::string convert_char_core_to_es_internal(const char* input)
{
	std::string temp = std::string(input);
	return convert_core_to_es_internal(temp);
}

Shader shader_compile_loaded_program(const char* vertexDataTemp, const char* fragmentDataTemp, const char* geometryData = nullptr)
{
	printf("NARROWED BUG DOWN TO THIS SEGMENT OF CODE!");
#ifdef EMSCRIPTEN
	std::string vertexDatastr = convert_char_core_to_es_internal(vertexDataTemp);
	std::string fragmentDatastr = convert_char_core_to_es_internal(fragmentDataTemp);
	const char* vertexData = vertexDatastr.c_str();
	const char* fragmentData = fragmentDatastr.c_str();
#else
	const char* vertexData = vertexDataTemp;
	const char* fragmentData = fragmentDataTemp;
#endif 
	//std::cout << vertexData << std::endl;
	//std::cout << fragmentData << std::endl;

	//create the shaders.
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::cout << "glShaderSource VERT CREATE" << glGetError() << std::endl; // returns 0 (no error)
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::cout << "glShaderSource FRAG CREATE" << glGetError() << std::endl; // returns 0 (no error)

	//compile the vertex shader.
	glShaderSource(vertexShader, 1, &vertexData, NULL);
	std::cout << "glShaderSource VERT LOAD" << glGetError() << std::endl; // returns 0 (no error)
	glCompileShader(vertexShader);
	std::cout << "glShaderSource VERT COMPILE" << glGetError() << std::endl; // returns 0 (no error)

	GLint success;
	char infoLog[1024];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	std::cout << "glGetShaderiv VERT" << glGetError() << std::endl; // returns 0 (no error)
	if (!success) {
		glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		std::cout << infoLog << std::endl;
		abort();
	}
	else
	{
		std::cout << "successfully attached vertex shader" << std::endl;
	}


	glShaderSource(fragmentShader, 1, &fragmentData, NULL);
	std::cout << "glShaderSource FRAG LOAD" << glGetError() << std::endl; // returns 0 (no error)
	glCompileShader(fragmentShader);
	std::cout << "glShaderSource FRAG COMPILE" << glGetError() << std::endl; // returns 0 (no error)

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		std::cout << infoLog << std::endl;
		abort();
	}
	else
	{
		std::cout << "successfully attached fragment shader" << std::endl;
	}

	Shader result = glCreateProgram();
	std::cout << "glCreateProgram: " << glGetError() << std::endl; // returns 0 (no error)
	glAttachShader(result, vertexShader);
	std::cout << "glAttachShader VERT: " << glGetError() << std::endl; // returns 0 (no error)
	glAttachShader(result, fragmentShader);
	std::cout << "glAttachShader FRAG: " << glGetError() << std::endl; // returns 0 (no error)
	glLinkProgram(result);
	std::cout << "LINK: " << glGetError() << std::endl; // returns 0 (no error)
	glGetProgramiv(result, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(result, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
		std::cout << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return result;
}

Shader shader_compile_program(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
{
	std::string vertexString = resource_load_text_file(vertexPath);
	const char* vertex = vertexString.c_str();
	std::string fragmentString = resource_load_text_file(fragmentPath);
	const char* frag = fragmentString.c_str();
	const char* geo = nullptr;
	if (geometryPath != nullptr)
		geo = resource_load_text_file(geometryPath).c_str();
	//std::cout << vertexString << std::endl;
	//std::cout << fragmentString << std::endl;
	return shader_compile_loaded_program(vertex, frag, geo);

}
void shader_use(Shader shader)
{
	glUseProgram(shader);
}
void shader_set_uniform_int(Shader shader, const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(shader, name.c_str()), value);
}
void shader_set_uniform_float(Shader shader, const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(shader, name.c_str()), value);
}
void shader_set_uniform_vec2(Shader shader, const std::string& name, glm::vec2 value)
{
	glUniform2f(glGetUniformLocation(shader, name.c_str()), value.x, value.y);
}
void shader_set_uniform_vec3(Shader shader, const std::string& name, glm::vec3 value)
{
	glUniform3f(glGetUniformLocation(shader, name.c_str()), value.x, value.y, value.z);
}
void shader_set_uniform_vec4(Shader shader, const std::string& name, glm::vec4 value)
{
	glUniform4f(glGetUniformLocation(shader, name.c_str()), value.x, value.y, value.z,value.w);
}
void shader_set_uniform_mat2(Shader shader, const std::string& name, glm::mat2 value)
{
	glUniformMatrix2fv(glGetUniformLocation(shader, name.c_str()), 1, false, glm::value_ptr(value));
}
void shader_set_uniform_mat3(Shader shader, const std::string& name, glm::mat3 value)
{
	glUniformMatrix3fv(glGetUniformLocation(shader, name.c_str()), 1, false, glm::value_ptr(value));
}
void shader_set_uniform_mat4(Shader shader, const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(shader, name.c_str()), 1, false, glm::value_ptr(value));
}