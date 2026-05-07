SOURCE_DIR := src
ADDITIONAL_SOURCES :=

OUT_DIR := out



dev : copy_assets
	gcc ./$(SOURCE_DIR)/main.c $(ADDITIONAL_SOURCES) \
	-o ./$(OUT_DIR)/game.exe \
	-I"./SDL3/include/" \
	-L"./SDL3/lib/"  -lSDL3 -lSDL3_ttf \


# add `-mwindows` flag to disable console
build : copy_assets
	gcc ./$(SOURCE_DIR)/main.c $(ADDITIONAL_SOURCES) \
	-o ./$(OUT_DIR)/game.exe \
	-I"./SDL3/include/" \
	-L"./SDL3/lib/"  -lSDL3 -lSDL3_ttf \
	-mwindows \
	-O2 -s



make_out_dir :
	if not exist "./$(OUT_DIR)" mkdir "./$(OUT_DIR)"

copy_assets : make_out_dir
	robocopy "./SDL3/bin" "./$(OUT_DIR)" "SDL3.dll" "SDL3_ttf.dll" /XC /XN /XO
	robocopy "./assets/fonts/JetBrains_Mono/static" "./$(OUT_DIR)" "JetBrainsMono-Regular.ttf" /XC /XN /XO
#	if not exist "./$(OUT_DIR)/SDL3.dll" copy "./SDL3/bin/SDL3.dll" "./$(OUT_DIR)"
#	if not exist "./$(OUT_DIR)/SDL3_ttf.dll" copy "./SDL3/bin/SDL3_ttf.dll" "./$(OUT_DIR)"



.PHONY : dev build make_out_dir copy_assets
