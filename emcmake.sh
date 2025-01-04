cmake . -GNinja -Bbuild/web \
 -DCMAKE_TOOLCHAIN_FILE=/home/sweetheart/Repos/Kohaku/buildscripts/emscripten/emsdk-portable/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
 -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=BOTH -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=BOTH \
 -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
 -DCMAKE_PREFIX_PATH=/home/sweetheart/Repos/Kohaku/buildscripts/emscripten -DCMAKE_BUILD_TYPE=Release \
 -DPLAYER_JS_BUILD_SHELL=ON -DPLAYER_JS_OUTPUT_NAME=index \
 -DPLAYER_JS_GAME_URL=/Games/