#ifndef __WEAPONS_H__
#define __WEAPONS_H__

#include "gfc_vector.h"
#include "entity.h"
#include "item.h"

Entity *fire_projectile(Vector2D origin, Vector2D velocity);

Entity create_hitbox(Vector2D origin, int duration);

void reload_weapon(Item *weapon);

void fire_pistol(Item *pistol, Vector2D player_position, float player_rotation);

void fire_shotgun(Item *shotgun, Vector2D player_position, float player_rotation);


#endif
