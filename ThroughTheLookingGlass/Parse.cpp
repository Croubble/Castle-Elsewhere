#include "Parse.h"
#include <iostream>
#ifdef EMSCRIPTEN
#define sprintf_s(args1,args2,...) sprintf(args1,__VA_ARGS__)
#endif
int distance_to_char(const char* pos, const char match)
{
	int i = 0;
	for (; pos[0] != match || pos[0] == '\0'; pos++)
	{
		i++;
	}
	return i;
}

int parse_number(const char* pos)
{
	int result = 0;
	for (; pos[0] >= '0' && pos[0] <= '9'; pos++)
	{
		result *= 10;
		int to_add = pos[0] - '0';
		result += to_add;
	}
	return result;
}

bool parse_strings_match(const char* pos, const char* match, int max_length)
{
	for (int i = 0; i < max_length; i++, pos++, match++)
	{
		if (pos[0] != match[0])
			return false;
	}
	return true;
}

int to_num(char c)
{
	return c - '0';
}

bool is_num_nonzero(char c)
{
	return c >= '1' && c <= '9';
}

bool is_num(char c)
{
	return c >= '0' && c <= '9';
}

bool is_whitespace(char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

int* try_parse_number(Tokenizer* t, Memory* temp_memory)
{
	int* result = (int*)memory_alloc(temp_memory, sizeof(int));
	char* on_fail = t->at;
	if (t->at[0] == '0')
	{
		t->at++;
		*result = 0;
		return result;
	}
	bool negative_number = false;
	if (t->at[0] == '-')
	{
		t->at++;
		negative_number = true;
	}
	if (is_num_nonzero(t->at[0]))
	{
		int num = to_num(t->at[0]);
		t->at++;
		while (is_num(t->at[0]))
		{
			num *= 10;
			num += to_num(t->at[0]);
			t->at++;
		}
		*result = num;
		if (negative_number)
			*result = -(*result);
		return result;
	}

	t->at = on_fail;
	return NULL;
}

bool try_parse_string(Tokenizer* t, const char* name)
{
	char* fail_result = t->at;

	while (t->at[0] && name[0])
	{
		if (t->at[0] != name[0])
		{
			t->at = fail_result;
			return false;
		}
		t->at++;
		name++;
	}
	//if we haven't reached the end of the string, success!
	if (t->at[0])
	{
		return true;
	}
	else
	{
		t->at = fail_result;
		return false;
	}
}

bool try_parse_char(Tokenizer* t, char c)
{
	if (t->at[0] == c)
	{
		t->at++;
		return true;
	}
	return false;
}

int* try_parse_number_comma_pair(Tokenizer* t, Memory* temp_memory)
{
	char* on_fail = t->at;
	int* result = try_parse_number(t, temp_memory);
	bool comma_after = try_parse_char(t, ',');
	if (comma_after && result)
	{
		return result;
	}
	t->at = on_fail;
	return NULL;

}

void consume_whitespace(Tokenizer* t)
{
	while (is_whitespace(t->at[0]))
		t->at++;
}

int* try_parse_num_gamestates(Tokenizer* t, Memory* temp_memory)
{
	char* on_fail = t->at;

	bool parse_finished = false;

	bool found_num_gamestates = try_parse_string(t, "num_gamestates:");
	if (found_num_gamestates)
	{
		int* maybe_num = try_parse_number(t, temp_memory);
		try_parse_char(t, ';');
		if (maybe_num)
		{
			return maybe_num;
		}
		else
		{
			std::cout << "Failed to read number of gamestates number. Oh no! We crashing now, this is real bad." << std::endl;
			abort();
		}
	}

	t->at = on_fail;
	return NULL;

}

bool try_parse_positions(Tokenizer* t, TimeMachineEditorStartState* result, Memory* temp_memory)
{
	char* on_fail = t->at;
	bool found_positions = try_parse_string(t, "positions:");
	if (found_positions)
	{
		bool reached_end = try_parse_char(t, ';');
		int num_positions_parsed = 0;
		while (!reached_end)
		{
			int* nextW = try_parse_number_comma_pair(t, temp_memory);
			int* nextH = try_parse_number_comma_pair(t, temp_memory);
			//TODO: check that !'ing something that ain't a boolean is okay.
			if (!nextW || !nextH)
			{
				std::cout << "okay. Okay. Wow. Our position array is ill formatted. Something gone wrong. Better crash." << std::endl;
				abort();
			}
			result->gamestates_positions[num_positions_parsed].x = *nextW;
			result->gamestates_positions[num_positions_parsed].y = *nextH;
			num_positions_parsed++;

			reached_end = try_parse_char(t, ';');
		}
		return true;
	}
	t->at = on_fail;
	return false;

}

bool try_parse_layer(Tokenizer* t, TimeMachineEditorStartState* result, Memory* final_memory, Memory* temp_memory)
{
	char* on_fail = t->at;
	bool is_layer_parse_start = try_parse_string(t, "layer");
	int* maybe_layer_num = try_parse_number(t, temp_memory);
	bool is_semicolon_at_end = try_parse_char(t, ':');
	if (is_layer_parse_start && maybe_layer_num && is_semicolon_at_end)
	{
		//determine the layer we are writing too, and make sure that layer is in range.
		int layer_num = *maybe_layer_num;
		if (layer_num >= GAME_NUM_LAYERS)
		{
			std::cout << "we are trying to write to a layer number that is bigger than should exist in our game. Crashing now!" << std::endl;
			abort();
		}

		//loop until we reach the ending semicolon.
		int num_gamestates = 0;
		while (!try_parse_char(t, ';'))
		{
			int* wp = try_parse_number_comma_pair(t, temp_memory);
			int* hp = try_parse_number_comma_pair(t, temp_memory);
			if (!wp || !hp)
			{
				std::cout << "parse layer failed on parsing w h." << std::endl;
				abort();
			}
			int w = *wp;
			int h = *hp;
			int len = w * h;
			if (result->gamestates[num_gamestates] == NULL)
				result->gamestates[num_gamestates] = gamestate_create(final_memory, w, h);
			for (int i = 0; i < len; i++)
			{
				int* nextp = try_parse_number_comma_pair(t, temp_memory);
				if (!nextp)
				{
					std::cout << "parse layer failed on parsing number." << std::endl;
					abort();
				}
				int next = *nextp;
				result->gamestates[num_gamestates]->layers[layer_num][i] = next;
			}

			num_gamestates++;
		}
		return true;
	}

	t->at = on_fail;
	return false;
}

bool try_parse_names(Tokenizer* t, TimeMachineEditorStartState* result, Memory* final_memory, Memory* temp_memory)
{
	char* on_fail = t->at;
	bool is_names_parse = try_parse_string(t, "names:");
	if (!is_names_parse)
	{
		t->at = on_fail;
		return false;
	}

	//while we've not reached the end.
	int num_gamestates = 0;
	while (!try_parse_char(t, ';'))
	{
		int i = 0;
		while (t->at[0] != ',')
		{
			if (i >= GAME_LEVEL_NAME_MAX_SIZE)
			{
				std::cout << "parse failed, we've run across a hideous, hideous thing, we've run across" << std::endl;
				abort();
			}
			result->names[num_gamestates * GAME_LEVEL_NAME_MAX_SIZE + i] = t->at[0];
			i++;
			t->at++;
		}
		int z = num_gamestates * GAME_LEVEL_NAME_MAX_SIZE;
		for (; i < (num_gamestates + 1) * GAME_LEVEL_NAME_MAX_SIZE; i++)
			result->names[z + i] = '\0';

		t->at++;
		num_gamestates++;
	}
	return true;
}

TimeMachineEditorStartState* parse_deserialize_timemachine(std::string input_string, Memory* final_memory, Memory* temp_memory)
{
	//strip all whitespace from the input.
	{
		// end_pos = std::remove(input_string.begin(), input_string.end(), ' ');
		//input_string.erase(end_pos, input_string.end());
		std::string::iterator end_pos = std::remove(input_string.begin(), input_string.end(), '\n');
		input_string.erase(end_pos, input_string.end());
	}

	TimeMachineEditorStartState* result = (TimeMachineEditorStartState*)memory_alloc(final_memory, sizeof(TimeMachineEditorStartState));
	for (int i = 0; i < MAX_NUMBER_GAMESTATES; i++)
	{
		result->gamestates[i] = NULL;
	}
	for (int i = 0; i < GAME_LEVEL_NAME_MAX_SIZE * MAX_NUMBER_GAMESTATES; i++)
	{
		result->names[i] = '\0';
	}
	//get input
	char* input = &(input_string[0]);
	Tokenizer tokenizer;
	tokenizer.at = input;

	while (tokenizer.at[0] != NULL)
	{
		consume_whitespace(&tokenizer);
		int* maybe_num_gamestates = try_parse_num_gamestates(&tokenizer, temp_memory);
		if (maybe_num_gamestates)
		{
			result->number_of_gamestates = *maybe_num_gamestates;
			continue;
		}
		bool parsed_positions = try_parse_positions(&tokenizer, result, temp_memory);
		bool parsed_layer = try_parse_layer(&tokenizer, result, final_memory, temp_memory);
		bool parsed_names = try_parse_names(&tokenizer, result, final_memory, temp_memory);
		if (!maybe_num_gamestates && !parsed_positions && !parsed_layer && !parsed_names)
		{
			std::cout << "uh oh, we've failed to parse something and the parsing isn't over. Better crash!" << std::endl;
			std::cout << "rest of tokenizer next line:" << std::endl;
			if (tokenizer.at[0] == NULL)
				std::cout << "turns out there is no tokenizer left, and we are completely done.Huh ? Whats going on then." << std::endl;
			else
				std::cout << tokenizer.at[0] << std::endl;
			std::cout << "parsed_positions:" << parsed_positions << std::endl;
			std::cout << "parsed_layer:" << parsed_layer << std::endl;
			std::cout << "parsed_names:" << parsed_names << std::endl;
			std::cout << "parsed_positions:" << (maybe_num_gamestates != NULL) << std::endl;
			abort();
		}

		consume_whitespace(&tokenizer);
		if (tokenizer.at[0] == NULL)
			std::cout << "We should quit right now, like straight outa town." << std::endl;
	}
	return result;
}

std::string parse_serialize_timemachine(TimeMachineEditor* timeMachine, Memory* final_memory, Memory* temp_memory)
{
	const int max_length = 10000;
	int output_consumed = 0;
	char* output = (char*)memory_alloc(temp_memory, sizeof(char) * max_length);
	const int num_gamestates = timeMachine->current_number_of_gamestates;

	//serialize some nice little numbers, like the number of gamestates.
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "num_gamestates:%d;\n", num_gamestates);
	}
	//serialize the gamestate positions.
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "positions:");
		for (int i = 0; i < num_gamestates; i++)
		{
			output_consumed += sprintf_s(output + output_consumed, max_length, "%d,%d,", timeMachine->gamestates_positions[i].x, timeMachine->gamestates_positions[i].y);
		}
		output_consumed += sprintf_s(output + output_consumed, max_length, ";\n");
	}
	//serialize all the gamestates layers.
	{
		for (int z = 0; z < GAME_NUM_LAYERS; z++)
		{
			output_consumed += sprintf_s(output + output_consumed, max_length, "layer%i:", z);
			for (int i = 0; i < num_gamestates; i++)
			{
				GameState* state = timeMachine->gamestates[i];
				//serialize the gamestate.
				{
					const int w = timeMachine->gamestates[i]->w;
					const int h = timeMachine->gamestates[i]->h;
					const int layer_len = w * h;
					output_consumed += sprintf_s(output + output_consumed, max_length, "%d,%d", w, h);
					for (int j = 0; j < layer_len; j++)
					{
						output_consumed += sprintf_s(output + output_consumed, max_length, ",%d", timeMachine->gamestates[i]->layers[z][j]);
					}
					output_consumed += sprintf_s(output + output_consumed, max_length, ",");
				}
			}
			//put a little ';' at the end of that serialized gamestate.
			output_consumed += sprintf_s(output + output_consumed, max_length, ";\n");
		}
	}
	//serialize the gamestates names.
	{
		output_consumed += sprintf_s(output + output_consumed, max_length, "names:");
		for (int i = 0; i < num_gamestates; i++)
		{
			output_consumed += sprintf_s(output + output_consumed, max_length, "%s,", &timeMachine->names[i * GAME_LEVEL_NAME_MAX_SIZE]);
		}
		output_consumed += sprintf_s(output + output_consumed, max_length, ";\n");
	}
	return std::string(output);
}
