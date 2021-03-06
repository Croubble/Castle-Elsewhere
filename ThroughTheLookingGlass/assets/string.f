#version 330 core
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
in vec4 finalTextColor;
uniform sampler2D texture1;

void main() 
{
	//vec4 color = texture(texture1,TexCoord);
	//FragColor = vec4(color.x + 0.2,color.y + 0.2,color.z + 0.2, color.w);
	//FragColor = texture(texture1,TexCoord);

	float my_sample = texture(texture1, TexCoord).r;
    FragColor = vec4(finalTextColor.r,finalTextColor.g,finalTextColor.b, my_sample);
}