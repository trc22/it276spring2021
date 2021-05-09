#ifndef __WINDOWS_COMMON_H__
#define __WINDOWS_COMMON_H__

#include "gf2d_windows.h"

Window *window_yes_no(char *text, void(*onYes)(void *),void(*onNo)(void *),void *yesData,void *noData);
Window *window_text_entry(char *question, char *defaultText, size_t length, void(*onOk)(void *),void(*onCancel)(void *));
Window *window_main_menu(char *text, void(*onStart)(void *), void(*onLoad)(void *), void(*onLevelEdit)(void *), void *startData, void *loadData, void *levelData);
Window *window_level_editor(char *text, void(*onTile)(void *), void(*onPlayer)(void *), void(*onEnemy)(void *), void(*onPickup)(void *), void(*onInteract)(void *), void *tileData, void *playerData, void *enemyData, void *pickupData, void *interactData);

#endif
