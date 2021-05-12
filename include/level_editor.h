#ifndef __LEVEL_EDITOR_H__
#define __LEVEL_EDITOR_H__

#include "level.h"

typedef enum
{
	LE_TILES,
	LE_PLAYER,
	LE_ENEMY,
	LE_PICKUP,
	LE_INTERACT,
	LE_BACKGROUND,
	LE_TEST
}LevelEditStatus;

void level_editor_init(Level *level);

void level_editor_make();

void level_editor_update();

Level *level_editor_test(Level *level);

#endif
