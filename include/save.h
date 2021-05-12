#ifndef __SAVE_H__
#define __SAVE_H__

#include "gfc_vector.h"
#include "level.h"

void save_new(char *current_level, Vector2D position);

Level *save_load_level(Level *level_old);

void save_load_player();

#endif
