#include "simple_logger.h"
#include "enemy.h"
#include "player.h"

void enemy_think_regular(Entity *self);

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
	ent->health = 50;
	ent->position = position;
	ent->think = enemy_think_regular;
	ent->_canCollide = true;
	return ent;
}

void enemy_think_regular(Entity *self)
{
	if (self->position.x - get_player()->position.x < 200)
	{
		awake(self);
	}
	else
		asleep(self);
}

void asleep(Entity *enemy)
{
	enemy->velocity = vector2d(0, 0);
}

void awake(Entity *enemy)
{
	if (get_player()->position.x > enemy->position.x)
		enemy->velocity.x = 1;
	else
		enemy->velocity.x = -1;
}

void die(Entity *enemy)
{
	entity_free(enemy);
}
