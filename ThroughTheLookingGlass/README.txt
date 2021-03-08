A simple readme for things I really should remember:

====================== How to add a new tile the game =================
1. Copy the drawn asset into the texturePacker project
2. Run the textureAtlasProject in the debugger, generating new assets.
3. copy the assets into the project
4. in ResourceLoadText.cpp add a new binding from the arts asset name to the gamespace asset name.

TODO: fix it so that we don't need to mess with any copy and paste fixing of the generated code.