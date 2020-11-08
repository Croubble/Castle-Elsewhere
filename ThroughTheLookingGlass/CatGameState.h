#pragma once

#pragma once

struct Immobile
{
	char floor_value : 3; //empty, wall, player_escape, player_target, crate_target.
	char gate_connector_value : 5;
};

struct Mobile
{
	char piece_value : 2; //nothing, player, crate.
	char push : 1;	//whether the crate can be pushed.
	char pull : 1; //whether the crate can be pulled.
	char side : 1; //wheter the crate can be side pulled.
	char rigid_value : 3; //what rigid body value this piece has. 0 is none, 1-7 is available (max 7 diff rigid crates).
};


struct NuGameStatePermanent
{
	int w;
	int h;
	Immobile* permanent;	//always len 1, contains elements.
};

void UpdateGameState(int x, int y, int w, int h, Immobile* permanent_arr, Mobile* arr, int length)
{
	int arr_len = w * h;
	//for each mobile element in array, apply the player move action to that element.
			
	//1:find the players current position.
		//2a:apply any pull moves.
		//2b:apply any push moves.
		//2c:apply any side moves.
		//3:propegate rigidbody moves.
		//4:for each moving square, cancel its movement if its blocked. If any square's movement was cancelled, repeat this step.
		//5:apply any remaining moves.
		//6:update crate link.

	}
}
