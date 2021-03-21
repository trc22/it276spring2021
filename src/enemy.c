#include "simple_logger.h"
#include "enemy.h"
#include "weapons.h"
#include "player.h"

void enemy_think_regular(Entity *self);
void enemy_collide(Entity *self, Entity *other);


Entity *spawn_enemy_regular(Vector2D position)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for enemy");
		return NULL;
	}
	ent->sprite = ent->sprite = gf2d_sprite_load_all("images/enemy.png", 44, 60, 1);
	vector2d_copy(ent->position, position);
	ent->type = 2; //enemy
	ent->health = 10;
	ent->position = position;
	ent->think = enemy_think_regular;
	ent->collide = enemy_collide;
	ent->collisionTimer = 15;
	ent->_canCollide = true;
	ent->_touchingTile = false;
	return ent;
}

void enemy_think_regular(Entity *self)
{
	if (self->position.x - get_player()->position.x < 500)
	{
		awake(self);
	}
	else
		asleep(self);
	if (self->collisionTimer == 15)
		self->_canCollide = true;
	else
	{
		self->_canCollide = false;
		self->collisionTimer++;
	}
}

void asleep(Entity *enemy)
{
	enemy->velocity = vector2d(0, 0);
}

void awake(Entity *enemy)
{
	if (get_player()->position.x > enemy->position.x)
	{
		enemy->velocity.x = 1;
		enemy->rotation.y = 0;
	}
	else
	{
		enemy->velocity.x = -1;
		enemy->rotation.y = 180;
	}

	if (!enemy->_touchingTile)
	{
		enemy->velocity.y = 1;
		if (enemy->velocity.x < 0 && enemy->last_collision.x > enemy->position.x)
			enemy->velocity.x = 0;
		else if (enemy->velocity.x > 0 && enemy->last_collision.x < enemy->position.x)
			enemy->velocity.x = 0;
	}
	else
	{
		enemy->_touchingTile = false;
	}
}

void enemy_phyiscs(Entity *enemy)
{

}


void enemy_collide(Entity *self, Entity *other)
{
	if (other->type == 0)
	{
		slog("touching player");
	}
	if (other->type == 5) //If hit by bullet
	{
		entity_damage(self, 10);
		entity_free(other);
		return;
	}
	if (other->type == 6) //If hit by knife hitbox 
	{
		entity_damage(self, 5);
		self->collisionTimer = 0;
		return;
	}
	if (other->type == 7) //If hit by dynamite
	{
		slog("collision dynamite throw");
		create_explosion(self->position, vector2d(256, 256), 10); //create exploision
		return;
	}

	if (other->type == 8) //If hit by explosion
	{
		slog("collision explosion");
		entity_damage(self, 100);
		return;
	}

}