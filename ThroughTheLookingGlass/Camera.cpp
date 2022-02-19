#include "Math.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "GameState.h"
glm::mat4 camera_make_matrix(GameSpaceCamera camera)
{
	return glm::ortho(camera.left, camera.right, camera.down, camera.up, camera.closePoint, camera.farPoint);
}
float camera_ratio(ViewPortCamera viewport)
{
	int height = viewport.up - viewport.down;
	int width = viewport.right - viewport.left;
	return (float) width / (float) height;
}
float move_capped_distance(float pos, float goal, float cap)
{
	float dist = goal - pos;
	if (dist < 0)
	{
		if (dist >= -cap)
			return goal;
		return pos - cap;
	}
	if (dist > 0)
	{
		if (dist <= cap)
			return goal;
		return pos + cap;
	}
	return pos;
}
GameSpaceCamera math_camera_reposition_towards_internal(GameSpaceCamera moving, GameSpaceCamera goal, float max_distance)
{
	GameSpaceCamera result;
	float left_move_request = goal.left - moving.left;
	float right_move_request = goal.right - moving.right;
	float up_move_request = goal.up - moving.up;
	float down_move_request = goal.down - moving.down;
	float x_move = (left_move_request + right_move_request) / 2.0f;
	float y_move = (up_move_request + down_move_request) / 2.0f;
	if (x_move > 0)
		x_move = minf(x_move, max_distance);
	if (y_move > 0)
		y_move = minf(y_move, max_distance);
	if (x_move < 0)
		x_move = maxf(x_move, -max_distance);
	if (y_move < 0)
		y_move = maxf(y_move, -max_distance);

	if (x_move == 0 && y_move == 0)
		return moving;

	result.left = moving.left + x_move;
	result.right = moving.right + x_move;
	result.up = moving.up + y_move;
	result.down = moving.down + y_move;
	result.closePoint = moving.closePoint;
	result.farPoint = moving.farPoint;
	return result;
}
GameSpaceCamera math_camera_zoom_towards_internal(GameSpaceCamera moving, GameSpaceCamera goal, float max_distance)
{
	GameSpaceCamera result;
	float current_height = moving.up - moving.down;
	float goal_height_jump = goal.up - goal.down;
	float goal_height;
	if (goal_height_jump - current_height > 0)
	{
		goal_height = minf(goal_height_jump, current_height + max_distance);
	}
	else if (goal_height_jump - current_height < 0)
	{
		goal_height = maxf(goal_height_jump, current_height - max_distance);
	}
	else
		goal_height = current_height;

	float ratio = goal_height / current_height;
	float camera_center_x = (moving.left + moving.right) / 2.0f;
	float camera_center_y = (moving.up + moving.down) / 2.0f;
	float camera_width = (moving.right - moving.left);
	float camera_height = (moving.up - moving.down);
	float new_camera_half_width = (camera_width / 2.0f) * ratio;
	float new_camera_half_height = (camera_height / 2.0f) * ratio;

	result.left = camera_center_x - new_camera_half_width;
	result.right = camera_center_x + new_camera_half_width;
	result.down = camera_center_y - new_camera_half_height;
	result.up = camera_center_y + new_camera_half_height;
	result.closePoint = moving.closePoint;
	result.farPoint = moving.farPoint;
	return result;
}



float smooth_start3(float t)
{
	return t * t * t;
}
float smooth_stop3(float t)
{
	return 1 - (1 - t) * (1 - t) * (1 - t);
}
float mix(float a, float b, float weight, float t)
{
	return a * (1 - weight) + b * weight;
}
float smooth_step3(float t)
{
	return mix(smooth_start3(t), smooth_stop3(t), t, t);
}

GameSpaceCamera math_camera_move_towards_lerp(GameSpaceCamera moving, GameSpaceCamera goal, float current_time,float goal_time)
{
	float t = current_time / goal_time;
	t = minf(t, 1);
	t = maxf(t, 0);
	t = smooth_step3(t);
	GameSpaceCamera result;

	float x_center_old = (moving.right + moving.left) / 2.0f;
	float y_center_old = (moving.up + moving.down )/ 2.0f;
	float x_center_next = (goal.right + goal.left) / 2.0f;
	float y_center_next = (goal.up + goal.down) / 2.0f;

	float x_center_now = x_center_old * (1 - t) + x_center_next * t;
	float y_center_now = y_center_old * (1 - t) + y_center_next * t;
	
	float zoom_old = moving.up - moving.down;
	float zoom_next = goal.up - goal.down;
	float zoom_now = zoom_old * (1 - t) + zoom_next * t;

	float ratio = (moving.right - moving.left) / (moving.up - moving.down);
	result.left = x_center_now - (zoom_now * ratio) / 2.0f;
	result.right = x_center_now + (zoom_now * ratio) / 2.0f;
	result.up = y_center_now + zoom_now / 2.0f;
	result.down = y_center_now - zoom_now / 2.0f;
	result.closePoint = moving.closePoint;
	result.farPoint = moving.farPoint;
	return result;
}
GameSpaceCamera math_camera_build(float camera_height, float screen_center_x, float screen_center_y, ViewPortCamera viewport)
{
	GameSpaceCamera result;

	float camera_left_x;
	float camera_bottom_y;
	float camera_right_x;
	float camera_top_y;
	//calculate camera's top and right positions.
	{
		int camera_view_width = viewport.right - viewport.left;
		int camera_view_height = viewport.up - viewport.down;
		float ratio = (float)camera_view_width / (float)camera_view_height;
		camera_left_x = screen_center_x - (camera_height * ratio) / 2.0f;
		camera_bottom_y = screen_center_y - (camera_height) / 2.0f;
		camera_right_x = camera_left_x + camera_height * ratio;
		camera_top_y = camera_bottom_y + camera_height;
	}

	result.left = camera_left_x;
	result.right = camera_right_x;
	result.up = camera_top_y;
	result.down = camera_bottom_y;
	result.closePoint = -20;
	result.farPoint = 20.0f;
	return result;
}
glm::vec2 math_convert_screenspace_to_pixelspace(IntPair screenSpacePosition, GameSpaceCamera camera, ViewPortCamera view, float gameHeight)
{
	glm::vec2 result;
	result.x = screenSpacePosition.x + camera.left * view.up / gameHeight;
	result.y = screenSpacePosition.y + camera.down * view.up / gameHeight;
	return result;
}
glm::vec2 math_convert_pixelspace_to_screenspace(IntPair pixelSpacePosition, GameSpaceCamera camera, ViewPortCamera view, float gameHeight)
{
	glm::vec2 result;
	result.x = pixelSpacePosition.x - camera.left * view.up / gameHeight;
	result.y = pixelSpacePosition.y - camera.down * view.up / gameHeight;
	return result;
}
glm::vec2 math_convert_screenspace_to_gamespace(IntPair screenSpacePosition, GameSpaceCamera camera, ViewPortCamera view, float gameHeight)
{
	//convert screenspace to pixelspace.
	glm::vec2 pixelspace = math_convert_screenspace_to_pixelspace(screenSpacePosition, camera, view, gameHeight);
	//convert pixelspace to gamespace.
	glm::vec2 result;
	float divide = math_gamespace_to_pixelspace_multiplier(view, gameHeight);
	result.x = pixelspace.x / divide;
	result.y = pixelspace.y / divide;
	//return gamespace.
	return result;
}

float math_camera_height(GameSpaceCamera c)
{
	return c.up - c.down;
}
float math_camera_width(GameSpaceCamera c)
{
	return c.right - c.left;
}
GameSpaceCamera* math_camera_break_into_grid(Memory* memory, GameSpaceCamera old, int w, int h)
{
	float old_w = old.right - old.left;
	float old_h = old.up - old.down;
	float next_w = old_w / ((float) w);
	float next_h = old_h / ((float)h);

	GameSpaceCamera* result = (GameSpaceCamera*)memory_alloc(memory, sizeof(*result) * w * h);
	int z = 0;
	for(int i = 0; i < w;i++)
		for (int j = 0; j < h; j++,z++)
		{
			result[z].left = old.left + next_w * i;
			result[z].right = result[z].left + next_w;
			result[z].down = old.down + next_h * j;
			result[z].up = result[z].down + next_h;
		}
	return result;
}
GameSpaceCamera math_camera_trim_all(GameSpaceCamera old, float trim_percent)
{
	trim_percent /= 2.0f;
	float h = old.up - old.down;
	float w = old.right - old.left;
	old.down = old.down + h * trim_percent;
	old.up = old.up - h * trim_percent;
	old.left = old.left + w * trim_percent;
	old.right = old.right - w * trim_percent;
	return old;
}
GameSpaceCamera math_camera_trim_bottom(GameSpaceCamera old, float trim_percent)
{
	float height = old.up - old.down;
	old.down = old.down + height * trim_percent;
	return old;
}
GameSpaceCamera math_camera_trim_top(GameSpaceCamera old, float trim_percent)
{
	float height = old.up - old.down;
	old.up = old.up - height * trim_percent;
	return old;
}
GameSpaceCamera math_camera_trim_left(GameSpaceCamera old, float trim_percent)
{
	float width = old.right - old.left;
	old.left = old.left + width * trim_percent;
	return old;
}
GameSpaceCamera math_camera_trim_right(GameSpaceCamera old, float trim_percent)
{
	float width = old.right - old.left;
	old.right = old.right - width * trim_percent;
	return old;
}
/*******************************************************************************/
glm::vec3 lerp(glm::vec3 start, glm::vec3 end, float l)
{
	float inv = 1.0f - l;
	return glm::vec3(start.x * inv + end.x * l, start.y * inv + end.y * l, start.z * inv + end.z * l);
}
/*******************************************************************************/
glm::mat4 math_translated_matrix(glm::vec3 translate)
{
	glm::mat4 result = glm::mat4(1.0);
	return glm::translate(result, translate);
}
glm::mat4 math_translated_scaled_matrix(glm::vec3 translate, glm::vec3 scale)
{
	glm::mat4 result = glm::mat4(1.0f);
	result = glm::translate(result, translate);
	result = glm::scale(result, scale);
	return result;
}
glm::mat4 math_line_matrix(glm::vec3 start, glm::vec3 end)
{
	{
		glm::vec3 adj_start = start + glm::vec3(0.5f,0.5f,0.0f);
		glm::vec3 adj_end = end + glm::vec3(0.5f,0.5f,0.0f);
		const float height = 0.2f; //how wide we want our line to be drawn, set to a constant 0.2f for now.

		glm::vec2 normalized = glm::normalize(glm::vec2(adj_end.x - adj_start.x, adj_end.y - adj_start.y));
		//a normalized vectors (x,y) values are (cos(theta),sin(theta), so we can just plug these numbers into the rotation matrix formula!
		glm::mat4 result;
		{
			//a scaled, rotated, and translated matrix, where we don't need to do a bunch of matrix multiplication every single time. Faster! Hopefully.
			//matrix layout is column constructed, started from the top left, so like:
			//0,4,8 ,12
			//1,5,9 ,13
			//2,6,10,14
			//3,7,11,14
			result = glm::mat4(
				glm::vec4((adj_end - adj_start), 0),
				glm::vec4(-height * normalized.y, height * normalized.x, 0, 0),
				glm::vec4(0, 0, 1, 0),
				glm::vec4(adj_start.x, adj_start.y, adj_start.z, 1)
			);
		}
		return result;
	}
}
float math_gamespace_to_pixelspace_multiplier(ViewPortCamera view, float gameHeight)
{
	float yDist = (float) view.up - (float) view.down;
	float result = yDist / gameHeight;
	return result;
}
float math_pixelspace_to_gamespace_multiplier(ViewPortCamera view, float gameHeight)
{
	return 1 / math_gamespace_to_pixelspace_multiplier(view, gameHeight);
}


