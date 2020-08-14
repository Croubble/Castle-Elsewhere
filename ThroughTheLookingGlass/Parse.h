#pragma once
#include "Editor.h"
#include <string>
#include "Memory.h"
#include <regex>
#include "GenericGamestate.h"

/*
*	Okay so here is our file format, the basic idea is:
*   [MyVar]: [... all the stuff];
*   is used to seperate every major kind of parse value. from there, once we've identified what were actually parsing, we have our own system.
*/
struct StringSegment
{
	char* start;
	int length;
};

enum SegmentType
{
	FULL_LINE,
	TEXT,

};

struct Tokenizer
{
	char* at;
};
int distance_to_char(const char* start, const char match);
int parse_number(const char* start);
bool parse_strings_match(const char* start, const char* match, int max_length);

int to_num(char c);
bool is_num_nonzero(char c);
bool is_num(char c);
bool is_whitespace(char c);
int* try_parse_number(Tokenizer* t, Memory* temp_memory);
bool try_parse_string(Tokenizer* t, const char* name);
bool try_parse_char(Tokenizer* t, char c);
int* try_parse_number_comma_pair(Tokenizer* t, Memory* temp_memory);
void consume_whitespace(Tokenizer* t);


int* try_parse_num_gamestates(Tokenizer* t, Memory* temp_memory);
bool try_parse_positions(Tokenizer* t, TimeMachineEditorStartState* result, Memory* temp_memory);
bool try_parse_layer(Tokenizer* t, TimeMachineEditorStartState* result, Memory* final_memory, Memory* temp_memory);
bool try_parse_names(Tokenizer* t, TimeMachineEditorStartState* result, Memory* final_memory, Memory* temp_memory);
TimeMachineEditorStartState* parse_deserialize_timemachine(std::string input_string, Memory* final_memory, Memory* temp_memory);


std::string parse_serialize_timemachine(TimeMachineEditor* timeMachine, Memory* final_memory, Memory* temp_memory);