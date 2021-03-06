#include "Regex.h"
#include "Parse.h"
/***************************************************************************/
/***************************INTERNAL FUNCTIONS AND DATA********************/

bool try_parse_until_string(Tokenizer* t, const char* name)
{
	char* fail = t->at;
	bool done = false;
	while (t->at[0] != '\0')
	{
		bool found_string = try_parse_string(t, name);
		if (found_string)
			return true;
		t->at++;
	}
	t->at = fail;
	return false;
}
char* try_parse_until_char(Tokenizer* t, char stop_char, Memory* temp_memory)
{

	char* fail = t->at;
	int length = 1;
	while (t->at[0] != stop_char)
	{
		if (t->at[0] == '\0')
		{
			t-> at = fail;
			return NULL;
		}
		length++;
		t->at++;
	}
	char* result = (char*) memory_alloc(temp_memory, sizeof(char) * length);
	for (int i = 0; i < length - 1; i++)
		result[i] = fail[i];
	result[length - 1] = '\0';
	return result;
}
int* try_parse_until_number(Tokenizer* t, Memory* temp_memory)
{
	char* fail = t->at;
	bool done = false;
	while (t->at[0] != '\0')
	{
		int* maybe_num = try_parse_number(t,temp_memory);
		if (maybe_num)
			return maybe_num;
		t->at++;
	}
	t->at = fail;
	return NULL;
}
std::string parse_between_quotes(Tokenizer* t, Memory* temp_memory)
{
	char* fail = t->at;
	bool passing = try_parse_char(t, '"');
	char* result = try_parse_until_char(t, '"',temp_memory);
	if (result == NULL)
	{
		std::cout << "failed to parse between quotes. aborting." << std::endl;
		abort();
	}
	return std::string(result);
}

/***************************************************************************/
/***************************EXTERNAL FUNCTIONS********************/

AtlasData* GetAtlasPosition(Memory* temp_memory, std::string atlas, get_atlas_number_from_name func_atlas)
{
	char* input = &(atlas[0]);
	AtlasData* result = (AtlasData*)memory_alloc(temp_memory, sizeof(AtlasData));
	result->length = 0;
	
	//step 1: get length
	{
		Tokenizer length_tokenizer;
		length_tokenizer.at = input;
		{
			bool found_next = true;
			while (found_next)
			{
				found_next = try_parse_until_string(&length_tokenizer, "\"filename\":");
				if (found_next)
					result->length++;
			}
		}
	}

	//step 2: get enum values and corrosponding x,y,w,h
	result->enum_corrospoding_values = (int*)memory_alloc(temp_memory, sizeof(int) * result->length);
	result->positions = (Position*)memory_alloc(temp_memory, sizeof(Position) * result->length);
	Tokenizer t;
	t.at = input;
	{
		for (int i = 0; i < result->length; i++)
		{
			//parse until "filename":
			bool found_next_filename = try_parse_until_string(&t, "\"filename\": ");
			std::string quoted_name = parse_between_quotes(&t,temp_memory);
			bool found_next_frame = try_parse_until_string(&t, "\"frame\": {");
			int* x = try_parse_until_number(&t,temp_memory);
			int* y = try_parse_until_number(&t, temp_memory);
			int* w = try_parse_until_number(&t, temp_memory);
			int* h = try_parse_until_number(&t, temp_memory);
			if (!found_next_filename || !found_next_frame || !x || !y || !w || !h)
			{
				std::cout << "failure in our brand new texture atlas parser, fix it." << std::endl;
				abort();
			}
			//eat whitespace
			//parse a "" encapsulated string, returning the string found.
				result->enum_corrospoding_values[i] = func_atlas(quoted_name);

			result->positions[i].x = *x;
			result->positions[i].y = *y;
			result->positions[i].w = *w;
			result->positions[i].h = *h;
		}
	}
	
	//step 3: get meta piece_data.
	{
		bool found_meta = try_parse_until_string(&t, "\"meta\"");
		bool found_size = try_parse_until_string(&t, "\"w\":");
		int* nextW = try_parse_until_number(&t,temp_memory);
		int* nextH = try_parse_until_number(&t,temp_memory);

		if (found_meta && found_size && nextW && nextH)
		{
			result->metaWidth = *nextW;
			result->metaHeight = *nextH;
		}
		else
		{
			std::cout << "Meta pass failed. Aborting." << std::endl;
			abort();
		}
		return result;
	}

}
