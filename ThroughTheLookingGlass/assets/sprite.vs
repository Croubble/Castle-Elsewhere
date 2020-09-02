#version 330 core
precision highp float;
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 positionOffset;
layout (location = 3) in vec4 atlasCoord;	//first two the x_start, second two the y_start
uniform mat4 viewProjectionMatrix;

out vec2 TexCoord;
void main()
{
	gl_Position = viewProjectionMatrix * vec4(pos.x + positionOffset.x,pos.y + positionOffset.y, pos.z + positionOffset.z,1.0);
	float xPos = inputTexCoord.x * atlasCoord.z + (1 - inputTexCoord.x) * atlasCoord.x;
	float yPos = inputTexCoord.y * atlasCoord.w + (1 - inputTexCoord.y) * atlasCoord.y;
	TexCoord = vec2(xPos,yPos);
}

