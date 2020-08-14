#pragma once
#include <glm\fwd.hpp>
#include <glad\glad.h>

struct ParticleSystem;
//setup particle system
ParticleSystem particle_setup(int max_num_particles, int num_particles_types, glm::vec4* coordinate_map, GLuint particle_texture);


void particle_draw(ParticleSystem system, int range_start,int length);
//void particle_send(ParticleSystem system);

//what is a particle?
//a texture ID (int)
//a 3d position 