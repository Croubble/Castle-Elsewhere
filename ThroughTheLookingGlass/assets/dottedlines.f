#version 330 core
precision highp float;

out vec4 FragColor;

in vec2 TexCoord;
in vec2 wh;

uniform float time;

float wrap(float f)
{
	return mod(step(f,0.0) + f,1.0);
}

float test(float value, float num_segments, float percent)
{
	float v = wrap(value);
	return step(mod(v,1.0/num_segments), (1.0/num_segments) * percent);
}
void main() 
{
	//define variables we will eventually pass in.
	float t = time * 0.15;

	float w = wh.x;
	float h = wh.y;

	float jut = 0.14;

	float num_segments = 16.0;
	float percent = 0.72;

	//begin algorithm.
	float x = TexCoord.x;
	float y = TexCoord.y;


	float total = w * 2.0 + h * 2.02 - jut * 4.0;
	
	float jutN = jut / total;

	float left_start = -jut;
	float left_end = h - jut;
	float up_start = left_end - jut;
	float up_end = left_end + w - jut;
	float right_start = up_end - jut;
	float right_end = up_end + h - jut;
	float down_start = right_end - jut;
	float down_end = right_end + w - jut;

	float is_left = step(x*w,jut);
	float is_right = step(w-jut,x*w);
	float is_down = step(y*h,jut);
	float is_up = step(h-jut,y*h);

	
	float left_start_norm = left_start / total;
	float left_end_norm = left_end / total;
	float up_start_norm = up_start / total;
	float up_end_norm = up_end / total;
	float right_start_norm = right_start / total;
	float right_end_norm = right_end / total;
	float down_start_norm = down_start / total;
	float down_end_norm = down_end / total;


	//okay, we have successfully calculated the values. Yes. Yes. Now, now comes converting this value into the lines.


	float left_edge_top = test(mix(left_start_norm,left_end_norm,(h - jut) / h) + t,num_segments,percent);
	float left_edge_bot = test(mix(left_start_norm,left_end_norm,jut / h) + t,num_segments,percent);
	float up_edge_left = test(mix(up_start_norm,up_end_norm,jut / w) + t,num_segments,percent);
	float up_edge_right = test(mix(up_start_norm,up_end_norm,(w - jut) / w) + t,num_segments,percent);
	float right_edge_top = test(mix(right_start_norm,right_end_norm, (h - jut) / h) + t,num_segments,percent);
	float right_edge_bot = test(mix(right_start_norm,right_end_norm,jut / h) + t,num_segments,percent);
	float down_edge_left = test(mix(down_start_norm,down_end_norm,(w - jut) / w) + t,num_segments,percent);
	float down_edge_right = test(mix(down_start_norm,down_end_norm,jut / w) + t,num_segments,percent);

	//final calc

	float left_top_unblocked = step(y,(h - jut) / h) + left_edge_top;
	float left_bot_unblocked = step(jut / h,y) + left_edge_bot;
	float up_left_unblocked = step(jut / w,x) + up_edge_left;
	float up_right_unblocked = step(x,(w - jut) / w) + up_edge_right;
	float right_up_unblocked = step(y,(h - jut) / h) + right_edge_bot;
	float right_bot_unblocked = step(jut / h, y) + right_edge_top;
	float down_left_unblocked = step(jut / w,x) + down_edge_left;
	float down_right_unblocked = step(x,(w - jut) / w) + down_edge_right;

	float value_horizontal = test(mix(left_start_norm,left_end_norm,y) + t,num_segments,percent) * is_left * left_top_unblocked * left_bot_unblocked
	+ 
	test(mix(right_start_norm,right_end_norm,1.0-y) + t,num_segments,percent) * is_right * right_up_unblocked * right_bot_unblocked;

	float value_vertical = test(mix(up_start_norm,up_end_norm,x) + t,num_segments,percent) * is_up * up_left_unblocked * up_right_unblocked
	+ 
	test(mix(down_start_norm,down_end_norm,1.0-x) + t,num_segments,percent) * is_down * down_right_unblocked * down_left_unblocked;

	float value = max(value_horizontal,value_vertical);
	value = wrap(value + t);

	float left_off = step(left_edge_top,y*h) * left_edge_top + step(y*h,left_edge_bot) * left_edge_bot;

	float sidesA = step(0.01,value_horizontal);
	float sidesB = step(0.01,value_vertical);
	FragColor = vec4(0.0,0.0,0.0, step(0.01,sidesA + sidesB));
}