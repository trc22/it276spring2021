#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "gf2d_entity.h"
#include "item.h"

Entity *weapon_fire_projectile(Vector2D position, Vector2D velocity, int damage);

void weapon_fire_pistol();

void weapon_init_particles(char *sprite);

#endif