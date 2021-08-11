#pragma once
#include "Math.h"
#include "PreludeIncludes.h"
#include "Constants.h"
#include "sprite.h"
enum Widget_Type
{
	WT_BUTTON,
	WT_LABEL,
	WT_LENGTH
};
struct Button
{
	char* text;
	void (*callback)(void* input);
};

struct Label
{
	char* text;
};

struct Widget
{
	Widget_Type type;
	AABB bounds;
	Widget* left_link;
	Widget* right_link;
	Widget* up_link;
	Widget* down_link;
	union u
	{
		Button button;
		Label label;
	};
	
};

struct Canvas
{
	Widget* ws;
	int widgets_max_length;
	int widgets_current_length;
};

Widget* create_button(const char* text, AABB bounds)
{
	crash_err("TODO");
	return NULL;
}
Widget* create_label(const char* text, AABB bounds)
{
	crash_err("TODO");
	return NULL;
}

void build_rect_menu(AABB* output, AABB size, int w, int h)
{
	int ele_width = size.w / w;
	int ele_height = size.h / h;
	int z = 0;
	for(int i = 0; i < w;i++)
		for (int j = 0; j < h; j++,z++)
		{
			output[i].x = size.x + i * ele_width;
			output[i].y = size.y + j * ele_height;
			output[i].w = ele_width;
			output[i].h = ele_height;
		}
}
void rect_menu_links(Widget* ws, int w, int h) /*sets the links of each widget to their neighbours*/
{
	//note: all this t2D and f2D is slower than just doing a good old fashioned i,j loop at same time as z, but eh. 
	for (int z = 0; z < w * h; z++)
	{
		IntPair d2 = t2D(z, w, h);
		IntPair left = d2;
		left.x -= 1;
		IntPair right = d2;
		right.x + 1;
		IntPair up = d2;
		up.y + 1;
		IntPair down = d2;
		down.y - 1;
		if(left.x >= 0)
			ws[z].left_link = &ws[f2D(left.x, left.y, w, h)];
		if(right.x < w)
			ws[z].right_link = &ws[f2D(right.x, right.y, w, h)];
		if (down.y >= 0)
			ws[z].down_link = &ws[f2D(down.x, down.y, w, h)];
		if (up.y < h)
			ws[z].up_link = &ws[f2D(up.x, up.y, w, h)];
	}
}



//what we are doing is writing out a 
void draw_widgets(Widget* ws, int len, AllWrite* draw)
{
	//draw the text.
}
