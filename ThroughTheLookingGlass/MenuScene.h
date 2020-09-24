#pragma once
#include <string>
#include "Memory.h"
typedef void (*ButtonCallback)();
struct MenuButton
{
	std::string button_text;
	ButtonCallback callback;
};
struct MenuScene
{
	int num_buttons;
	MenuButton* buttons;
	int current_highlighted_button;
};

MenuScene* setup_main_menu(Memory* menu_memory, ButtonCallback new_game, ButtonCallback continue_game, ButtonCallback level_editor)
{
	memory_clear(menu_memory);

	MenuScene* result = (MenuScene*) memory_alloc(menu_memory, sizeof(MenuScene*));
	result->num_buttons = 3;
	result->buttons = (MenuButton*) memory_alloc(menu_memory, sizeof(MenuButton) * 3);
	result->buttons[0].button_text = "New Game";
	result->buttons[1].button_text = "Continue";
	result->buttons[2].button_text = "Level Editor";
	result->buttons[0].callback = new_game;
	result->buttons[1].callback = continue_game;
	result->buttons[2].callback = level_editor;
	result->current_highlighted_button = 1;
	return result;
}

