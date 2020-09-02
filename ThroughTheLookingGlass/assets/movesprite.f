#version 330 core
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
in vec2 UnadjustedTexCoord;
in vec4 color;
uniform sampler2D texture1;

void main() 
{
	vec4 tex_color = texture(texture1,TexCoord);
	vec4 final_color = vec4(tex_color.x * color.x, tex_color.y * color.y, tex_color.z * color.z, tex_color.w * color.w);
	FragColor = final_color;
}

//okay so the basic problem is that the invisible sprite we are drawing is preventing other sprites from been drawn. 
//the other problem is that our old system of just moving the tex_coord is gonna make things... trickier.
//solutions: do some z axis sorting, make sure that the start has a higher z value than the end.
//solution: do all the texture atlas texcoord work in here instead of .f, use my old chill system of just adding to the texcoord.
//so, provide the min_max from movesprite.vs, then add lerped between min and max, then if out of bounds don't draw. Then they stay static, but provide move illusion.