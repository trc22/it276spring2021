#include "simple_logger.h"
#include "enemy.h"
#include "weapons.h"
#include "player.h"

void enemy_think_regular(Entity *self);
void enemy_think_small(Entity *self);
void enemy_think_big(Entity *self);
void enemy_think_tall(Entity *self);
void enemy_think_ranged(Entity *self);

void awake_small(Entity *enemy);
void awake_big(Entity *enemy);
void awake_tall(Entity *enemy);
void awake_ranged(Entity *enemy);


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
		awake_regular(self);
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

Entity *spawn_enemy_small(Vector2D position)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for enemy");
		return NULL;
	}
	ent->sprite = ent->sprite = gf2d_sprite_load_all("images/enemy_small.png", 32, 46, 1);
	vector2d_copy(ent->position, position);
	ent->type = 2; //enemy
	ent->health = 10;
	ent->position = position;
	ent->think = enemy_think_small;
	ent->collide = enemy_collide;
	ent->collisionTimer = 15;
	ent->_canCollide = true;
	ent->_touchingTile = false;
	return ent;
}

void enemy_think_small(Entity *self)
{
	if (self->position.x - get_player()->position.x < 300)
	{
		awake_small(self);
	}
	else
		asleep(self);
}

void awake_small(Entity *enemy)
{
	if (get_player()->position.x > enemy->position.x)
	{
		enemy->velocity.x = 2;
		enemy->rotation.y = 0;
	}
	else
	{
		enemy->velocity.x = -2;
		enemy->rotation.y = 180;
	}

	if (!enemy->_touchingTile)
	{
		enemy->velocity.y = 1;
		enemy->velocity.x = 0;
	}

	else
	{
		enemy->_touchingTile = false;
	}
}


Entity *spawn_enemy_big(Vector2D position)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for enemy");
		return NULL;
	}
	ent->sprite = ent->sprite = gf2d_sprite_load_all("images/enemy_big.png", 128, 128, 1);
	vector2d_copy(ent->position, position);
	ent->type = 2; //enemy
	ent->health = 40;
	ent->position = position;
	ent->think = enemy_think_big;
	ent->collide = enemy_collide;
	ent->collisionTimer = 15;
	ent->_canCollide = true;
	ent->_touchingTile = false;
	return ent;
}

void enemy_think_big(Entity *self)
{
	if (self->position.x - get_player()->position.x < 500)
	{
		awake_big(self);
	}
	else
		asleep(self);
}

void awake_big(Entity *enemy)
{
	if (get_player()->position.x > enemy->position.x)
	{
		enemy->velocity.x = .5;
		enemy->rotation.y = 0;
	}
	else
	{
		enemy->velocity.x = -.5;
		enemy->rotation.y = 180;
	}

	if (!enemy->_touchingTile)
	{
		enemy->velocity.y = 2;
		enemy->velocity.x = 0;
	}

	else
	{
		enemy->_touchingTile = false;
	}
}

Entity *spawn_enemy_tall(Vector2D position)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for enemy");
		return NULL;
	}
	ent->sprite = ent->sprite = gf2d_sprite_load_all("images/enemy_tall.png", 70, 96, 1);
	vector2d_copy(ent->position, position);
	ent->type = 2; //enemy
	ent->health = 10;
	ent->position = position;
	ent->think = enemy_think_tall;
	ent->collide = enemy_collide;
	ent->collisionTimer = 15;
	ent->_canCollide = true;
	ent->_touchingTile = false;
	return ent;
}

void enemy_think_tall(Entity *self)
{
	if (self->position.x - get_player()->position.x < 500 && get_light_on())
	{
		self->_canCollide = true;
		awake_tall(self);
	}
	else
	{
		self->_canCollide = false;
		asleep(self);
	}
}

void awake_tall(Entity *enemy)
{
	if (get_player()->position.x > enemy->position.x)
	{
		enemy->velocity.x = .75;
		enemy->rotation.y = 0;
	}
	else
	{
		enemy->velocity.x = -.75;
		enemy->rotation.y = 180;
	}

	if (!enemy->_touchingTile)
	{
		enemy->velocity.y = 1;
		enemy->velocity.x = 0;
	}

	else
	{
		enemy->_touchingTile = false;
	}
}

Entity *spawn_enemy_ranged(Vector2D position)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for enemy");
		return NULL;
	}
	ent->sprite = ent->sprite = gf2d_sprite_load_all("images/enemy_ranged.png", 44, 62, 1);
	vector2d_copy(ent->position, position);
	ent->type = 2; //enemy
	ent->health = 10;
	ent->position = position;
	ent->think = enemy_think_ranged;
	ent->collide = enemy_collide;
	ent->_canCollide = false;
	ent->_touchingTile = false;
	ent->duration = 200; //Time inbetween ranged attacks
	return ent;
}

void enemy_think_ranged(Entity *self)
{
	if (self->position.x - get_player()->position.x < 500)
	{
		awake_ranged(self);
	}
	else
		asleep(self);
}

void awake_ranged(Entity *enemy)
{
	if (enemy->duration == 200)
	{
		if (get_player()->position.x > enemy->position.x)
		{
			fire_projectile_enemy(enemy->position, vector2d(1, 0));
		}
		else
		{
			fire_projectile_enemy(enemy->position, vector2d(-1, 0));
		}
		enemy->duration = 0;
	}
	else(enemy->duration++);
}


void asleep(Entity *enemy)
{
	enemy->velocity = vector2d(0, 0);
}

void awake_regular(Entity *enemy)
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
