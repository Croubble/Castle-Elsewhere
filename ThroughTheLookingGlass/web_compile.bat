CD BUILD
del build.zip
del write_castle.html
del index.html
cd ..
call emcc World.cpp Animation.cpp SaveLoad.cpp Click.cpp AABB.cpp Camera.cpp EditorTimeMachine.cpp GameState.cpp Grid.cpp IntPair.cpp Memory.cpp Parse.cpp Regex.cpp ResourceLoadText.cpp Shader.cpp Sprites.cpp Walk.cpp win32_platform.cpp^
 -s USE_SDL=2 -s USE_FREETYPE=1 -s SDL2_IMAGE_FORMATS=["png"] -g -s --preload-file assets/ -I"D:\OpenGL\Include" -s ASSERTIONS=1 -s INITIAL_MEMORY=268435456 -s MAX_WEBGL_VERSION=2 -s ALLOW_MEMORY_GROWTH=1 --shell-file custom_shell.html -O2 -o Build/write_castle.html
ren "build\write_castle.html" index.html
CD BUILD
7z a build.zip *
CD ..
butler push . dazed-pidgeon/prototype-emscripten-baaaaa:web
