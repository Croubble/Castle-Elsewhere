#pragma once

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#define strcpy_s strcpy
#include <SDL_image.h>
#include <SDL_ttf.h>
#define GL_GLES_PROTOTYPES 1
#define GL_GLEXT_PROTOTYPES 1
//#include <SDL_opengles2.h>
#include <GLES3/gl3.h>
#else
#define EM_BOOL bool 
#include <glad/glad.h>
#include "Clock.h"
#endif