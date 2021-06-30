#version 330 core
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
in vec4 color;
uniform sampler2D texture1;

void main() 
{
	FragColor =  texture(texture1,TexCoord) * color;
}