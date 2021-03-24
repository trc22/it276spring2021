#include "simple_logger.h"
#include "interactables.h"
#include "level.h"

void door_collide(Entity *self, Entity *other);
void button_collide(Entity *self, Entity *other);

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
	ent->duration = 1; //target level id
	ent->_usable = false;

	return ent;
}

void door_collide(Entity *self, Entity *other)
{
	if (self->_usable && other->type == 0)
	{
		if (check_inventory(3)) //if player has key
		{
			slog("using door");
			level_transition(get_current_level(), self);
		}
		else
			slog("You need a key");
	}
}

Entity *spawn_button(Vector2D position, const char *destination)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for button");
		return NULL;
	}

	ent->sprite = gf2d_sprite_load_all("images/button.png", 50, 26, 1);
	vector2d_copy(ent->position, position);

	ent->type = 21; //Type 21 = button
	//ent->last_collision.x = 0;
	//ent->last_collision.y = 0;

	ent->collide = button_collide;
	ent->_touchingWall = false;
	ent->_canCollide = true;
	ent->collisionTimer = 50; //using this for collisions

	ent->_usable = false;

	ent->destination = destination;
	return ent;
}

void button_collide(Entity *self, Entity *other)
{
	if (self->_usable && other->type == 0)
	{
		if (self->destination == "level_00_door")
		{
			spawn_door(vector2d(1800, 1730), "levels/exampleLevel.json");
			slog("A secret door has been revealed");
			entity_free(self);
		}
	}
}