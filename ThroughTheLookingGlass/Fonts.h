#pragma once

//TODO: Figure out how we are actually going to write fonts. 
//remember to look at openGL fonts stuff, its pretty helpful.
//maybe pos by just using a standard gylph font model? Honestly, it could be a really excellent placeholder, last a long while and let us 
//get a bunch of stuff out the way.

//okay so lets think about the plan like this. 
//0. get texture atlas to actually properly work. introduce a beefier shader with rotation and scaling.
//1. get a glyph sheet filled with all the letters, in order. 
//2. Want to display a font? What's the interface look like.
//3: well, we could just say an array of characters, that seems fine.
//4: so we have a little utility that just lets us draw a single character, and then we have "draw text" which repeatedly adds that to our gpu.
//its basically just drawing sprites.a sprite.
//5: 

//void write_text_to_gpu(string toWrite, vector3 startPosition, float height_in_game_units, SOME_ARBITRARY_DATA_LOCATION where we write too.)