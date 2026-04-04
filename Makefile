SOURCE_DIR := source
OUT_DIR := out

SDL_SOURCE := SDL/x86_64-w64-mingw32
ADDITIONAL_SOURCES := ./$(SOURCE_DIR)/rxi/vec-0.2.1/src/vec.c


build : compile copy_SDL_dll

# add `-mwindows` flag to disable console
compile : make_out_dir
	gcc ./$(SOURCE_DIR)/main.c $(ADDITIONAL_SOURCES) \
	-o ./$(OUT_DIR)/game.exe \
	-I"./$(SDL_SOURCE)/include/" \
	-L"./$(SDL_SOURCE)/lib/"  -lSDL3 \
	-O2 -s

copy_SDL_dll : make_out_dir
	if not exist "./$(OUT_DIR)/SDL3.dll" copy "./$(SDL_SOURCE)/bin/SDL3.dll" "./$(OUT_DIR)/"
#robocopy "./$(SDL_SOURCE)/bin/" "./$(OUT_DIR)/" "SDL3.dll" /XC /XN /XO

make_out_dir :
	if not exist "./$(OUT_DIR)" mkdir "./$(OUT_DIR)"

clean :
	if exist "./$(OUT_DIR)" rmdir /s "./$(OUT_DIR)"


.PHONY : build compile copy_SDL_dll make_out_dir clean
