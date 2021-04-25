#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "gf2d_entity.h"

Entity *spawn_enemy_regular(Vector2D position);

Entity *spawn_enemy_small(Vector2D position);

Entity *spawn_enemy_big(Vector2D position);

Entity *spawn_enemy_tall(Vector2D position);

Entity *spawn_enemy_ranged(Vector2D position);

void die(Entity *enemy);



#endif
