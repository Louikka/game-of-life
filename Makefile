SOURCE_DIR := src
ADDITIONAL_SOURCES := $(SOURCE_DIR)/grid.c

BUILD_DIR := build



arch : make_build_dir
	gcc arch.c -o ./$(BUILD_DIR)/arch.exe


dev : make_build_dir
	gcc ./$(SOURCE_DIR)/main.c $(ADDITIONAL_SOURCES) \
	-o ./$(BUILD_DIR)/game-dev.exe \
	-I"./SDL3-3.4.8/x86_64-w64-mingw32/include/" -I"./SDL3_ttf-3.2.2/x86_64-w64-mingw32/include/" \
	-L"./SDL3-3.4.8/x86_64-w64-mingw32/lib/"  -lSDL3 \
	-L"./SDL3_ttf-3.2.2/x86_64-w64-mingw32/lib/"  -lSDL3_ttf \


# add `-mwindows` flag to disable console
build : make_build_dir
	gcc ./$(SOURCE_DIR)/main.c $(ADDITIONAL_SOURCES) \
	-o ./$(BUILD_DIR)/game.exe \
	-I"./SDL3-3.4.8/x86_64-w64-mingw32/include/" -I"./SDL3_ttf-3.2.2/x86_64-w64-mingw32/include/" \
	-L"./SDL3-3.4.8/x86_64-w64-mingw32/lib/"  -lSDL3 \
	-L"./SDL3_ttf-3.2.2/x86_64-w64-mingw32/lib/"  -lSDL3_ttf \
	-mwindows \
	-O2 -s \



make_build_dir :
	if not exist "./$(BUILD_DIR)" mkdir "./$(BUILD_DIR)"



.PHONY : arch dev build make_build_dir
