#pragma once
#include <string>
#include "glm/glm.hpp"

typedef int (*gui_function)(int);
struct GUI
{

};

struct GUI_Text
{
	std::string text;
	glm::vec3 position;
	glm::vec2 scale;
};

struct GUI_Button
{
	std::string text;
	glm::vec3 position;
	glm::vec2 scale;
	glm::vec4 color;	//color to draw the button background.
	gui_function function;
};
struct GUI_Buttons
{
	std::string* texts;
	glm::vec3* positions;
	glm::vec2 scales;
	glm::vec4 colors;
	gui_function* functions;
};
struct GUI_Menu
{
	int length;
	int button_selected;
	GUI_Button* buttons;
};

int calculate_button_clicked_readonly(GUI_Button* buttons, glm::vec2 mousepos)
{
	return -1;
}