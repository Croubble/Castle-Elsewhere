#version 330 core
precision highp float;
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec4 atlas;
layout (location = 3) in vec4 textColor;
layout (location = 4) in mat4 positionScaleRotate;

uniform mat4 viewProjectionMatrix;

out vec2 TexCoord;
out vec4 finalTextColor;
void main()
{
	gl_Position = viewProjectionMatrix * positionScaleRotate * vec4(pos,1.0);

	float xPos = texCoords.x * atlas.z + (1.0 - texCoords.x) * atlas.x;
	float yPos = texCoords.y * atlas.w + (1.0 - texCoords.y) * atlas.y;
	TexCoord = vec2(xPos,yPos);
	finalTextColor = textColor;
	//gl_Position = vec4((pos.x * 2.0 - 1.0) * 1.0, (pos.y * 2.0 - 1.0) * 1.0,pos.z,1.0);
	//TexCoord = texCoords;
}

