#version 330 core
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
in vec4 color;
uniform sampler2D texture1;

void main() 
{
	vec4 texColor = texture(texture1,TexCoord);
	vec4 result = vec4(color.r * texColor.r, color.g * texColor.g, color.b * texColor.b, color.a * texColor.a);
	FragColor =  texture(texture1,TexCoord);
}