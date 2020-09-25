#pragma once
#include <string>
#include "Memory.h"
#include "TextDraw.h"
typedef void (*ButtonCallback)();
struct MenuButton
{
	int blah;
	char* button_text;
	ButtonCallback callback;
};
struct MenuScene
{
	int num_buttons;
	int current_highlighted_button;
	MenuButton* buttons;
};

char* char_alloc(Memory* memory, std::string to_alloc)
{
	int length = to_alloc.length() + 1;
	char* result = (char*) memory_alloc(memory, sizeof(char) * length);
	for (int i = 0; i < to_alloc.length(); i++)
		result[i] = to_alloc[i];
	result[to_alloc.length()] = NULL;
	return result;
}
MenuScene* setup_main_menu(Memory* menu_memory, ButtonCallback new_game, ButtonCallback continue_game, ButtonCallback level_editor)
{
	memory_clear(menu_memory);

	MenuScene* result = (MenuScene*) memory_alloc(menu_memory, sizeof(MenuScene));
	result->num_buttons = 3;
	result->current_highlighted_button = 1;
	result->buttons = (MenuButton*) memory_alloc(menu_memory, sizeof(MenuButton) * 3);
	char* first = char_alloc(menu_memory, "New Game");
	result->buttons[0].button_text = first;
	result->buttons[1].button_text = char_alloc(menu_memory,"Continue");
	result->buttons[2].button_text = char_alloc(menu_memory,"Level Editor");
	result->buttons[0].callback = new_game;
	result->buttons[1].callback = continue_game;
	result->buttons[2].callback = level_editor;

	return result;
}


