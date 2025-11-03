SRC_DIR := source
OUT_DIR := out

RAYLIB_DIR := $(SRC_DIR)/raylib-5.5

SOURCE_FILES := $(SRC_DIR)/main.c $(SRC_DIR)/timer.c


build : make_dir_if
	gcc $(SOURCE_FILES) \
	-o $(OUT_DIR)/a.exe \
	-I $(RAYLIB_DIR)/include  -L $(RAYLIB_DIR)/lib  -lraylib \
	-lgdi32 -lwinmm -mwindows  -O2 -s


make_dir_if :
	if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)"


.PHONY : build make_dir_if copy_dll_if clean
