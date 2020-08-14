#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;

uniform vec4 offset;
out vec2 TexCoord;

void main()
{
	//gl_Position = vec4(offset.x + pos.x * offset.z,offset.y + pos.y * offset.w,0.0,1.0);
	vec4 top_half_position = vec4(offset.x + pos.x * offset.z,offset.y + pos.y * offset.w,0.0,1.0);
	gl_Position = vec4(top_half_position.x * 2 - 1, top_half_position.y * 2 - 1,top_half_position.z,top_half_position.w);
	TexCoord = vec2(texCoords.x,1.0 - texCoords.y);
}

