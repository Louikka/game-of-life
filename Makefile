SOURCE_DIR := src
OUT_DIR := out

SDL_SOURCE := SDL3
ADDITIONAL_SOURCES :=


build : compile copy_assets

# add `-mwindows` flag to disable console
compile : make_out_dir
	gcc ./$(SOURCE_DIR)/main.c $(ADDITIONAL_SOURCES) \
	-o ./$(OUT_DIR)/game.exe \
	-I"./$(SDL_SOURCE)/include/" \
	-L"./$(SDL_SOURCE)/lib/"  -lSDL3 \
	-mwindows \
	-O2 -s

copy_assets : make_out_dir
#robocopy "./$(SDL_SOURCE)/bin/" "./$(OUT_DIR)/" "SDL3.dll" /XC /XN /XO
	if not exist "./$(OUT_DIR)/SDL3.dll" copy "./$(SDL_SOURCE)/bin/SDL3.dll" "./$(OUT_DIR)/"

make_out_dir :
	if not exist "./$(OUT_DIR)" mkdir "./$(OUT_DIR)"

clean :
	if exist "./$(OUT_DIR)" rmdir /s "./$(OUT_DIR)"


.PHONY : build compile copy_assets make_out_dir clean
