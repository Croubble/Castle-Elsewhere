#pragma once
#include<glm/glm.hpp>

#include <string>
#include "PreludeIncludes.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef GLuint Shader;

Shader shader_compile_program(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
void shader_use(Shader shader);	//just a wrapping for gl_use for util.

void shader_set_uniform_int(Shader shader, const std::string& name, int value);
void shader_set_uniform_float(Shader shader, const std::string& name, float value);
void shader_set_uniform_vec2(Shader shader, const std::string& name, glm::vec2 value);
void shader_set_uniform_vec3(Shader shader, const std::string& name, glm::vec3 value);
void shader_set_uniform_vec4(Shader shader, const std::string& name, glm::vec4 value);
void shader_set_uniform_mat2(Shader shader, const std::string& name, glm::mat2 value);
void shader_set_uniform_mat3(Shader shader, const std::string& name, glm::mat3 value);
void shader_set_uniform_mat4(Shader shader, const std::string& name, glm::mat4 value);