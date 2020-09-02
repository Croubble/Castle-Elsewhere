#version 330 core
precision highp float;
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 positionOffset;
layout (location = 3) in vec4 atlasCoord;	//first two the x_start, second two the y_start
layout (location = 4) in vec2 movementoffset;
layout (location = 5) in vec4 colorOffset;
uniform mat4 viewProjectionMatrix;

out vec2 TexCoord;
out vec2 UnadjustedTexCoord;
out vec4 color;
//out vec2 nonAtlasTexCoord;
void main()
{
	//gl_Position = viewProjectionMatrix * vec4(pos.x + positionOffset.x,pos.y + positionOffset.y, pos.z + positionOffset.z,1.0);
	//float xPos = inputTexCoord.x * atlasCoord.z + (1 - inputTexCoord.x) * atlasCoord.x;
	//float yPos = inputTexCoord.y * atlasCoord.w + (1 - inputTexCoord.y) * atlasCoord.y;
	//TexCoord = vec2(xPos,yPos);
	//nonAtlasTexCoord = vec2(movementoffset.x,movementoffset.y);

	float x = pos.x + movementoffset.x;
	float y = pos.y + movementoffset.y;
	float xb = max(0.0,min(1.0,x));
	float yb = max(0.0,min(1.0,y));
	float x_tex = pos.x - movementoffset.x;
	float y_tex = pos.y - movementoffset.y;
	float x_texb =  max(0.0,min(1.0,x_tex));
	float y_texb =  max(0.0,min(1.0,y_tex));

	gl_Position = viewProjectionMatrix * vec4(xb + positionOffset.x,yb + positionOffset.y, pos.z + positionOffset.z,1.0);
	vec2 true_input = vec2(x_texb,y_texb);
	//vec2 true_input = vec2(inputTexCoord.x - movementoffset.x,inputTexCoord.y - movementoffset.y);
	float xPos = true_input.x * atlasCoord.z + (1.0 - true_input.x) * atlasCoord.x;
	float yPos = true_input.y * atlasCoord.w + (1.0 - true_input.y) * atlasCoord.y;
	TexCoord = vec2(xPos,yPos);
	UnadjustedTexCoord = true_input;
	color = colorOffset;
}

//todo: there positions, the moving sprite positions, stay static the whole time. the offset of texture reading moves.