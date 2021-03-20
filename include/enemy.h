#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "entity.h"

Entity *spawn_enemy_regular(Vector2D position);

void asleep(Entity *enemy);

void awake(Entity *enemy);

void die(Entity *enemy);

#endif
