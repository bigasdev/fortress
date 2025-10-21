# set the App name
NAME = rpg_side_screen
# set compiler
CC = g++ -std=c++17 -Wno-narrowing
STATIC_LIBS = -static-libstdc++ -static-libgcc
INCLUDES = -Iflag/src -Isrc -Iinclude -Iinclude/sdl -Iinclude/headers -Llib -Llibs -Lflag/libs -lflag
LIBS = -lstdc++fs -lflag -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gpu -lSDL2_mixer -lole32 -mwindows -lflag
DEBUG_LIBS = -lstdc++fs -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gpu -lSDL2_mixer -lpsapi -lpdh -lole32 -mwindows -lflag
# set all the files needed
DEBUG_FOLDER = .build
RELEASE_FOLDER = .release

ICON_DIR = res/icon/icon.res
BIN = bin/*.o
DEBUG_FLAGS = -D_ASSERTS -D_DEBUG -D_IMGUI
GAME_FLAGS = -D_IMGUI -DWIN_WIDTH=1024 -DWIN_HEIGHT=768 -DGAME_SCALE=3

bin_dir:
	mkdir bin

SYSTEMS_DIRS := src/systems/health src/systems/physics src/systems/farming src/systems/farming/popup src/systems/collision src/systems/player src/systems/weapon src/systems/sprite

SRC_DIRS := $(SYSTEMS_DIRS) src/imgui src/core src/core/ui src/messages src/components src/systems src/entity src/entity/ui src/renderer src/res src/game src/tools

OBJ_FILES := $(foreach dir, $(SRC_DIRS), $(patsubst $(dir)/%.cpp, bin/%.o, $(wildcard $(dir)/*.cpp)))

vpath %.cpp $(SRC_DIRS)

bin/%.o: %.cpp
	$(CC) $(GAME_FLAGS) $(if $(filter true,$(DEBUG)),-g $(DEBUG_FLAGS)) $(INCLUDES) -MMD -MP -c $< -o $@

debug: $(OBJ_FILES)
	${CC} -g -O0 $(STATIC_LIBS) $(INCLUDES) -o $(DEBUG_FOLDER)/fortress.exe $(OBJ_FILES) $(ICON_DIR) $(DEBUG_LIBS) -mconsole

build: $(OBJ_FILES)
	${CC} -s -O3 -finline-functions -flto $(STATIC_LIBS) $(INCLUDES) -o $(RELEASE_FOLDER)/${NAME}.exe $(OBJ_FILES) $(ICON_DIR) $(LIBS)

compile: bin_dir $(OBJ_FILES)
	${CC} $(STATIC_LIBS) $(INCLUDES) -Llib -o ${NAME}_debug $(OBJ_FILES) $(ICON_DIR) $(LIBS) -mconsole

web: src/core/*.cpp src/entity/*.cpp src/renderer/*.cpp src/res/*.cpp src/game/*.cpp src/tools/*.cpp
	em++ $^ -o .web/yare.js -g -lm --bind -std=c++20 -Wno-narrowing -s USE_SDL=2 -O3 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s SDL2_IMAGE_FORMATS='["png"]' -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_STACK=64MB -s INITIAL_MEMORY=96MB --preload-file res --preload-file res/font --preload-file res/icon --preload-file res/sprites --use-preload-plugins

-include $(OBJ_FILES:.o=.d)
