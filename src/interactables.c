#include "simple_logger.h"
#include "interactables.h"
#include "level.h"

void door_collide(Entity *self);

Entity *spawn_door(Vector2D position, const char *destination)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for door");
		return NULL;
	}

	ent->sprite = gf2d_sprite_load_all("images/door.png", 64, 64, 1);
	vector2d_copy(ent->position, position);

	ent->type = 22; //Type 22 = door
	//ent->last_collision.x = 0;
	//ent->last_collision.y = 0;
	ent->collide = door_collide;
	ent->_canJump = false;

	ent->_touchingWall = false;
	ent->_canCollide = true;
	ent->collisionTimer = 50; //using this for collisions
	
	ent->destination = destination;

	ent->_usable = false;

	return ent;
}

void door_collide(Entity *self, Entity *other)
{
	if (self->_usable && other->type == 0)
	{
		slog("using door");
		level_transition(get_current_level(), self);
	}
}

void lever();
