#version 330 core
precision highp float;
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 positionOffset;
layout (location = 2) in vec2 scale;
uniform mat4 viewProjectionMatrix;

out vec2 TexCoord;
out vec2 wh;
void main()
{
	gl_Position = viewProjectionMatrix * vec4(pos.x * scale.x + positionOffset.x,pos.y * scale.y + positionOffset.y, pos.z + positionOffset.z,1.0);
	TexCoord = vec2(pos.x,pos.y);
	wh = vec2(scale.x,scale.y);
}