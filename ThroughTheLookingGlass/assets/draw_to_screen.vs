#version 330 core
layout (location = 0) in vec2 TexCoords; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}