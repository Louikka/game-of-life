SRC_DIR := source
OUT_DIR := out

RAYLIB_DIR := $(SRC_DIR)/raylib-5.5

SOURCE_FILES := $(SRC_DIR)/main.c $(SRC_DIR)/timer.c


.PHONY : build make_dir_if clean


# add `-mwindows` flag to disable console

build : make_dir_if
	gcc $(SOURCE_FILES) \
	-o $(OUT_DIR)/game.exe \
	-I $(RAYLIB_DIR)/include  -L $(RAYLIB_DIR)/lib  -lraylib \
	-lgdi32 -lwinmm -mwindows \
	-O2 -s


make_dir_if :
	if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)"
