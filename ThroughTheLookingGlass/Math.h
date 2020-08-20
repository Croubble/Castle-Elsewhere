#pragma once

#include "Memory.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

/******************************IntPair   ****************************************/
/********************************************************************************/

//in game space, i.e. the unit of measurement we using, where are the points.
//we pass this to glm::ortho to make the mat4 camera we actually send to the GPU to make transformations.
struct GameSpaceCamera
{
	float left;
	float right;
	float up;
	float down;
	float closePoint;
	float farPoint;
};

//This says, on the screen, what is the bottom left corner, and what is the top right corner. e.g. for a 800x600 screen, 0,800,0,600 is the viewport camera.
struct ViewPortCamera
{
	int left;
	int right;
	int up;
	int down;
};
struct IntPair
{
	int x;
	int y;
};

IntPair math_intpair_create(int x, int y);
IntPair math_intpair_add(IntPair left, IntPair right);
IntPair math_intpair_sub(IntPair left, IntPair right);
bool math_intpair_eq(IntPair left, IntPair right);
/*******************************SCREEN LOGIC************************************/
float camera_ratio(ViewPortCamera viewport);
glm::mat4 camera_make_matrix(GameSpaceCamera camera);
GameSpaceCamera math_camera_move_towards_lerp(GameSpaceCamera moving, GameSpaceCamera goal, float current_time, float goal_time);
GameSpaceCamera math_camera_build(float camera_height, float screen_center_x, float screen_center_y, ViewPortCamera viewport);
float math_pixelspace_to_gamespace_multiplier(ViewPortCamera view, float gameHeight);
float math_gamespace_to_pixelspace_multiplier(ViewPortCamera view, float gameHeight);
glm::vec2 math_screenspace_to_pixelspace(IntPair screenSpacePosition, GameSpaceCamera camera, ViewPortCamera view, float gameHeight);
glm::vec2 math_pixelspace_to_screenspace(IntPair pixelSpacePosition, GameSpaceCamera camera, ViewPortCamera view, float gameHeight);
glm::vec2 math_screenspace_to_gamespace(IntPair screenSpacePosition, GameSpaceCamera camera, ViewPortCamera view, float gameHeight);
/*******************************************************************************/
/***************************** SIDE ********************************************/
/*******************************************************************************/

enum Side
{
	S_RIGHT,
	S_DOWN,
	S_LEFT,
	S_UP,
	S_COUNT
};

IntPair math_side_to_direction(Side s);
Side math_side_opposite(Side s);
Side math_direction_to_side(IntPair pair);

/******************************** GRID LOGIC ************************************/
/********************************************************************************/

IntPair math_grid_move_direction(IntPair pos, IntPair move, IntPair dimensions);
IntPair math_grid_move_direction(int xStart, int yStart, int xMove, int yMove, int w, int h);
IntPair math_grid_move_direction_wrapped(IntPair pos, IntPair move, IntPair dimensions);
IntPair math_grid_move_direction_wrapped(int xStart, int yStart, int xMove, int yMove, int w, int h);

bool* math_grid_walk_adjacent(Memory* result_memory, Memory* temp_memory, int* input_grid, int start_x, int start_y, int w, int h);
bool* math_grid_walk_adjacent_wrapped(Memory* result_memory, Memory* temp_memory, int* input_grid, int start_x, int start_y, int w, int h);

/**********************************COLLISION LOGIC*******************************/
/********************************************************************************/

struct AABB
{
	float x;
	float y;
	float w;
	float h;
};

AABB math_AABB_create(float x, float y, float width, float height);
int math_click_is_inside_AABB_list(float clickX, float clickY, AABB* boxes, int total_boxes);

bool math_AABB_is_colliding(AABB test, AABB* against_array, int length);
void math_AABB_break_into_borders(AABB box, AABB* output, float border_size);

inline bool math_click_is_inside_AABB(float clickX, float clickY, AABB box)
{
	return clickX >= box.x && clickX < (box.x + box.w) && clickY >= box.y && clickY < (box.y + box.h);
}
inline float percent_between_two_points(float point, float min, float max)
{
	return (point - min) / (max - min);
}
inline bool math_click_is_inside_AABB(float left, float bottom, float right, float top, float clickX, float clickY)
{
	return clickX >= left && clickX < right && clickY >= bottom && clickY < top;
}
inline bool math_click_is_inside_AABB(IntPair min, IntPair max, IntPair click)
{
	return click.x >= min.x && click.x <= max.x && click.y >= min.y && click.y <= max.y;
}


/************************************* ARRAY LOGIC ******************************/
/********************************************************************************/

//from 2 dimension to 1 dimension.
inline int f2D(int x, int y, int w, int h)
{
	return y + x * h;
}

//from 1 dimension to 2 dimensions.
inline IntPair t2D(int i, int w, int h)
{
	IntPair result;
	result.x = i / h;
	result.y = i % h;
	return result;
}

inline bool math_within_grid(int x, int y, int w, int h)
{
	return x >= 0 && y >= 0 && x < w && y < h;
}
/************************************MISC FUNCTIONS INLINE************************/

inline float maxf(float x, float y) {
	if (x < y)
		return y;
	return x;
}
inline float minf(float x, float y)
{
	if (x < y)
		return x;
	return y;
}
inline int max(int x, int y)
{
	if (x >= y)
		return x;
	return y;
}
inline int min(int x, int y)
{
	if (x <= y)
		return x;
	return y;
}

inline int sign(float x)
{
	if (x > 0)
		return 1;
	if (x < 0)
		return -1;
	return 0;
}
