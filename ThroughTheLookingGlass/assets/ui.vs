#version 330 core
precision highp float;
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec4 atlasCoord;
layout (location = 3) in vec4 myInputColor;
layout (location = 4) in mat4 positionScaleRotate;	//a matrix that takes into account position, scale, and rotation. replaces just a flat position.


uniform mat4 viewProjectionMatrix;

out vec2 TexCoord;
out vec4 color;
void main()
{
	//gl_Position = viewProjectionMatrix * vec4(pos,1.0);
	//TexCoord = inputTexCoord;
	gl_Position = viewProjectionMatrix * positionScaleRotate * vec4(pos,1.0);

	float xPos = inputTexCoord.x * atlasCoord.z + (1.0 - inputTexCoord.x) * atlasCoord.x;
	float yPos = inputTexCoord.y * atlasCoord.w + (1.0 - inputTexCoord.y) * atlasCoord.y;
	TexCoord = vec2(xPos,yPos);
	color = myInputColor;
}