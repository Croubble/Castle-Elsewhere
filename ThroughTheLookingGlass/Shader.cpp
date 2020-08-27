#include <string>

#include "PreludeIncludes.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Resource.h"
#include <iostream>

typedef GLuint Shader;

Shader shader_compile_loaded_program(const char* vertexData, const char* fragmentData, const char* geometryData = nullptr)
{

	//bool useGeometryShader = geometryData != nullptr;

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::cout << "glShaderSource VERT CREATE" << glGetError() << std::endl; // returns 0 (no error)
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::cout << "glShaderSource FRAG CREATE" << glGetError() << std::endl; // returns 0 (no error)
	glShaderSource(vertexShader, 1, &vertexData, NULL);
	std::cout << "glShaderSource VERT LOAD" << glGetError() << std::endl; // returns 0 (no error)
	glCompileShader(vertexShader);
	std::cout << "glShaderSource VERT COMPILE" << glGetError() << std::endl; // returns 0 (no error)


	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	std::cout << "glGetShaderiv VERT" << glGetError() << std::endl; // returns 0 (no error)
	if (!success) {
		glGetProgramInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	std::cout << fragmentData[0] << fragmentData[1] << fragmentData[2] << std::endl;

	glShaderSource(fragmentShader, 1, &fragmentData, NULL);
	std::cout << "glShaderSource FRAG LOAD" << glGetError() << std::endl; // returns 0 (no error)
	glCompileShader(fragmentShader);
	std::cout << "glShaderSource FRAG COMPILE" << glGetError() << std::endl; // returns 0 (no error)

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//unsigned int geometryShader;
	//if (useGeometryShader)
	//{
	//	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	//	glShaderSource(geometryShader, 1, &geometryData, NULL);
	//	glCompileShader(geometryShader);
	//}

	Shader result = glCreateProgram();
	std::cout << "glCreateProgram" << glGetError() << std::endl; // returns 0 (no error)
	glAttachShader(result, vertexShader);
	std::cout << "glCreateProgram VERT" << glGetError() << std::endl; // returns 0 (no error)
	glAttachShader(result, fragmentShader);
	std::cout << "glCreateProgram FRAG" << glGetError() << std::endl; // returns 0 (no error)
	//if(useGeometryShader)
	//	glAttachShader(result, geometryShader);
	glLinkProgram(result);
	std::cout << "LINK" << glGetError() << std::endl; // returns 0 (no error)
	glGetProgramiv(result, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(result, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//if (useGeometryShader)
	//	glDeleteShader(geometryShader);

	std::cout << "Shader result" << result << std::endl;
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