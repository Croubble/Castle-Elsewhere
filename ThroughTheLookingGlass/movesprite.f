#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 UnadjustedTexCoord;
uniform sampler2D texture1;

void main() 
{
	//float x = nonAtlasTexCoord.x;
	//float y = nonAtlasTexCoord.y;
	//float use_black_color = step(1,nonAtlasTexCoord.x) + step(1,nonAtlasTexCoord.y) + step(x,-0.01) + step(y,-0.01);
	//use_black_color = step(0.1,use_black_color);
	//if(use_black_color > 0.1)
	//	discard;

	//float no_draw = step(1,UnadjustedTexCoord.x) + step(1,UnadjustedTexCoord.y) + step(UnadjustedTexCoord.x,0) + step(UnadjustedTexCoord.y,0);
	//no_draw = step(0.1,no_draw);
	//FragColor =  texture(texture1,TexCoord) * (1.0 - no_draw);

	FragColor =  texture(texture1,TexCoord);
}

//okay so the basic problem is that the invisible sprite we are drawing is preventing other sprites from been drawn. 
//the other problem is that our old system of just moving the tex_coord is gonna make things... trickier.
//solutions: do some z axis sorting, make sure that the start has a higher z value than the end.
//solution: do all the texture atlas texcoord work in here instead of .f, use my old chill system of just adding to the texcoord.
//so, provide the min_max from movesprite.vs, then add lerped between min and max, then if out of bounds don't draw. Then they stay static, but provide move illusion.