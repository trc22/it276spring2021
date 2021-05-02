#ifndef __PICKUP_H__
#define __PICKUP_H__

#include "gf2d_entity.h"
#include "item.h"

typedef struct Pickup_s
{
	Bool		_inuse;
	char		*name;
	int			id;
	Entity		*entity;
	Sprite		*sprite;
	Item		*item;
	Vector2D	position;
}Pickup;

void init_pickup_system(Uint32 maxPickups);

void pickup_system_close();

Pickup *pickup_spawn(Vector2D location, char *pickup_name, int pickup_id, char* pickup_sprite, int item_id);

Pickup *pickup_new();

Pickup *pickup_get_by_id(int pickup_id);

void pickup_respawn_ent(Pickup *pickup);

void pickup_free(Pickup *pickup);

void pickup_insert_fail(Item *item);


#endif