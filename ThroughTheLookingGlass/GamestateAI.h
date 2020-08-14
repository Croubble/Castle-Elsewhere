#pragma once

typedef short int word;

namespace GamestateAI
{
	struct Gamestate
	{
		int w;
		int h;
		int num_crates;
		bool* horizontal_walls;
		bool* vertical_walls;
		int mouse_target;
	};

	template<typename size_t>
	void expand(Gamestate state, const int input_length, size_t* input, size_t* output)
	{
		//expand up action.
		for (int i = 0; i < input_length; i++)
		{

		}
		//expand right action.
		for (int i = 0; i < input_length; i++)
		{

		}
		//expand left action.
		for(int i = 0; i < input_length;i++)
		{

		}
		//expand down action
		for (int i = 0; i < input_length; i++)
		{

		}
	}
}