#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "entity.h"

Entity *spawn_enemy_regular(Vector2D position);

Entity *spawn_enemy_small(Vector2D position);

Entity *spawn_enemy_big(Vector2D position);

Entity *spawn_enemy_tall(Vector2D position);

Entity *spawn_enemy_ranged(Vector2D position);

void asleep(Entity *enemy);

void awake(Entity *enemy);

void die(Entity *enemy);

void enemy_phyiscs(Entity *enemy);



#endif
