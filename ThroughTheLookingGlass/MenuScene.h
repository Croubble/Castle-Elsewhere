#pragma once
#include <string>
#include "Memory.h"
typedef void (*ButtonCallback)();
struct MenuButton
{
	char* button_text;
	ButtonCallback callback;
};
struct MenuScene
{
	int num_buttons;
	MenuButton* buttons;
	int current_highlighted_button;
};

char* char_alloc(Memory* memory, std::string to_alloc)
{
	char* result = (char*) memory_alloc(memory, sizeof(char) * (to_alloc.length() + 1));
	for (int i = 0; i < to_alloc.length(); i++)
		result[i] = to_alloc[i];
	result[to_alloc.length()] = NULL;
	return result;
}
MenuScene* setup_main_menu(Memory* menu_memory, ButtonCallback new_game, ButtonCallback continue_game, ButtonCallback level_editor)
{
	memory_clear(menu_memory);

	MenuScene* result = (MenuScene*) memory_alloc(menu_memory, sizeof(MenuScene*));
	result->num_buttons = 3;
	result->buttons = (MenuButton*) memory_alloc(menu_memory, sizeof(MenuButton) * 3);
	result->buttons[0].button_text = char_alloc(menu_memory,"New Game");
	result->buttons[1].button_text = char_alloc(menu_memory,"Continue");
	result->buttons[2].button_text = char_alloc(menu_memory,"Level Editor");
	result->buttons[0].callback = new_game;
	result->buttons[1].callback = continue_game;
	result->buttons[2].callback = level_editor;
	result->current_highlighted_button = 1;
	return result;
}

