#ifndef GAME_OF_LIFE_TEXT_H
#define GAME_OF_LIFE_TEXT_H



extern TTF_Font *Font_JetBrainsMonoRegular;

bool LoadFonts();

void CloseFonts();


extern TTF_Text *TextObj_Controls;

bool CreateTextObjects(TTF_TextEngine *engine);

void DestroyTextObjects();



#endif
