OUT_DIR := out


build : compile copy_SDL_dll

# add `-mwindows` flag to disable console
compile : make_out_dir
	gcc main.c \
	-o $(OUT_DIR)/game.exe \
	-I ./SDL/x86_64-w64-mingw32/include/ \
	-L ./SDL/x86_64-w64-mingw32/lib/  -lSDL3 \
	-mwindows \
	-O2 -s

copy_SDL_dll : make_out_dir
	if not exist $(OUT_DIR)/SDL3.dll cp "./SDL/x86_64-w64-mingw32/bin/SDL3.dll" "$(OUT_DIR)/"

make_out_dir :
	if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)"


.PHONY : build compile copy_SDL_dll make_out_dir clean
