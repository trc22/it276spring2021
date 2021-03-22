#ifndef __INTERACTABLES_H__
#define __INTERACTABLES_H__

#include "entity.h"

Entity *spawn_door(Vector2D position, const char *destination);

Entity *spawn_button(Vector2D position, const char *destination);


#endif