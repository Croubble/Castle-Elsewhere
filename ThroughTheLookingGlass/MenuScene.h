#pragma once
#include <string>
#include "Memory.h"
#include "TextDraw.h"
typedef void (*ButtonCallback)(void* input);

struct MenuLabel
{
	char* text;
};
struct MenuButton
{
	ButtonCallback callback;
	char* text;
	int left_focus;
	int right_focus;
	int up_focus;
	int down_focus;
};
struct Menu
{
	int current_highlighted_button;
	int num_buttons;
	int num_labels;
	MenuButton* buttons;
	MenuLabel* labels;
	
};

struct Keybinding_Scene
{
	Menu menu;
	bool waiting_for_keybind;
	int button_keybinding;
};

void menu_left_action(Menu* menu)
{
	int focus = menu->buttons[menu->current_highlighted_button].left_focus;
	if (focus >= 0 && focus < menu->num_buttons)
	{
		menu->current_highlighted_button = focus;
	}
}
void menu_right_action(Menu* menu)
{
	int focus = menu->buttons[menu->current_highlighted_button].right_focus;
	if (focus >= 0 && focus < menu->num_buttons)
	{
		menu->current_highlighted_button = focus;
	}
}
void menu_up_action(Menu* menu)
{
	int focus = menu->buttons[menu->current_highlighted_button].up_focus;
	if (focus >= 0 && focus < menu->num_buttons)
		menu->current_highlighted_button = focus;
}
void menu_down_action(Menu* menu)
{
	int focus = menu->buttons[menu->current_highlighted_button].down_focus;
	if (focus >= 0 && focus < menu->num_buttons)
		menu->current_highlighted_button = focus;
}

void menu_setup_button_focuses(MenuButton* buttons, int w, int h)
{
	for (int z = 0; z < w * h; z++)
	{
		buttons[z].left_focus = -1;
		buttons[z].right_focus = -1;
		buttons[z].up_focus = -1;
		buttons[z].down_focus = -1;
	}
	int z = 0;
	for(int i = 0; i < w;i++)
		for (int j = 0; j < h; j++,z++)
		{
			if (i >= 0)
				buttons[z].left_focus = f2D(i - 1, j,w,h);
			if (i < w - 1)
				buttons[z].right_focus = f2D(i + 1, j, w, h);
			if (j >= 0)
				buttons[z].down_focus = f2D(i, j - 1, w, h);
			if (j < h - 1)
				buttons[z].up_focus = f2D(i, j + 1, w, h);
		}
}
char* char_alloc(Memory* memory, std::string to_alloc)
{
	int length = to_alloc.length() + 1;
	char* result = (char*) memory_alloc(memory, sizeof(char) * length);
	for (int i = 0; i < to_alloc.length(); i++)
		result[i] = to_alloc[i];
	result[to_alloc.length()] = NULL;
	return result;
}
MenuButton setup_button(Memory* menu_memory, const char* name, ButtonCallback on_click)
{
	MenuButton result;
	result.text = char_alloc(menu_memory, name);
	result.callback = on_click;
	printf("%p\n", on_click);
	return result;
}
const char** menu_get_all_labels_text(Menu* menu, Memory* memory)
{
	const char** result = (const char**)memory_alloc(memory, sizeof(*result) * menu->num_labels);
	int len = menu->num_labels;
	for (int i = 0; i < len;i++)
	{
		result[i] = menu->labels[i].text;
	}
	return result;
}
const char** menu_get_all_buttons_text(Menu* menu, Memory* memory)
{
	const char** result = (const char**)memory_alloc(memory, sizeof(*result) * menu->num_buttons);
	int len = menu->num_buttons;
	for (int i = 0; i < len;i++)
	{
		result[i] = menu->buttons[i].text;
	}
	return result;
}
void keybinding_log_menu(Keybinding_Scene* scene)
{
	for (int i = 0; i < scene->menu.num_buttons; i++)
	{
		printf("%p\n", scene->menu.buttons[i].callback);
	}
}
Keybinding_Scene* setup_keybinding_menu(Memory* keybinding_memory, ButtonCallback keybind_action, MappingState* key_mapping)
{
	memory_clear(keybinding_memory);

	Keybinding_Scene* result = (Keybinding_Scene*) memory_alloc(keybinding_memory, sizeof(Keybinding_Scene));
	result->menu.num_buttons = G_LENGTH * 2;
	result->menu.num_labels = G_LENGTH;
	result->menu.buttons = (MenuButton*) memory_alloc(keybinding_memory, sizeof(MenuButton) * result->menu.num_buttons);  
	result->menu.labels = (MenuLabel*)memory_alloc(keybinding_memory, sizeof(MenuLabel) * result->menu.num_labels);
	for(int i = 0; i < G_LENGTH;i++)
	{
		//todo: why is this not working? My suspision is that this char array is somehow overwriting the pointer. Le sigh.
		result->menu.buttons[i] = setup_button(keybinding_memory, SDL_GetKeyName(key_mapping->primary_mapping[i]), keybind_action);
	}
	for (int i = 0;i < G_LENGTH;i++)
	{
		result->menu.buttons[i + G_LENGTH] = setup_button(keybinding_memory, SDL_GetKeyName(key_mapping->secondary_mapping[i]), keybind_action);
	}
	for (int i = 0; i < G_LENGTH; i++)
	{
		GAME_ACTION_NAME foo = click_gameaction_to_name((GAME_ACTION)i);
		result->menu.labels[i].text = char_alloc(keybinding_memory,foo.name);
	}
	result->menu.current_highlighted_button = 0;
	result->waiting_for_keybind = false;
	result->button_keybinding = -1;
	menu_setup_button_focuses(result->menu.buttons, 2, G_LENGTH);

	return result;
}
Menu* setup_main_menu(Memory* menu_memory, ButtonCallback new_game, ButtonCallback continue_game, ButtonCallback level_editor, ButtonCallback keybinding)
{
	memory_clear(menu_memory);

	Menu* result = (Menu*) memory_alloc(menu_memory, sizeof(Menu));
	result->num_buttons = 4;
	result->current_highlighted_button = 1;

	int i = 0;
	int expected_buttons = 4;
	result->buttons = (MenuButton*) memory_alloc(menu_memory, sizeof(MenuButton) * expected_buttons);
	result->buttons[i++] = setup_button(menu_memory, "New Game", new_game);
	result->buttons[i++] = setup_button(menu_memory,"Continuey",continue_game);
	result->buttons[i++] = setup_button(menu_memory,"Level Editor",level_editor);
	result->buttons[i++] = setup_button(menu_memory, "Keybinding",keybinding);
	if (i != expected_buttons)
		crash_err("go update expected buttons");
	menu_setup_button_focuses(result->buttons, 1, 4);
	return result;
}


