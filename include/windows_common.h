#ifndef __WINDOWS_COMMON_H__
#define __WINDOWS_COMMON_H__

#include "gf2d_windows.h"

Window *window_yes_no(char *text, void(*onYes)(void *),void(*onNo)(void *),void *yesData,void *noData);
Window *window_text_entry(char *question, char *defaultText, size_t length, void(*onOk)(void *),void(*onCancel)(void *));
Window *window_main_menu(char *text, void(*onStart)(void *), void(*onLoad)(void *), void(*onQuit)(void *), void *startData, void *loadData, void *quitData);

#endif
