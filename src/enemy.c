#include "simple_logger.h"
#include "enemy.h"
//#include "weapons.h"
#include "player.h"
//#include "overlay.h"

void enemy_think_regular(Entity *self);
void enemy_touch_regular(Entity *self, Entity *other);

int enemy_damage(Entity *self, int amount, Entity *source);
void enemy_die(Entity *self);

void enemy_seek(Entity *self, Entity *target);

Entity *spawn_enemy_regular(Vector2D position)
{
	Entity *ent;
	ent = entity_spawn("actors/enemy_regular.actor", "enemy_regular", position);
	ent->body.team = 1;
	ent->id = 1;

	ent->health = 20;

	ent->think = enemy_think_regular;
	ent->touch = enemy_touch_regular;
	ent->damage = enemy_damage;
	ent->die = enemy_die;
	return ent;
}

void enemy_think_regular(Entity *self)
{
	if (vector2d_distance_between_less_than(self->position, get_player()->position, 500))
	{
		self->state = ES_Seeking;
		enemy_seek(self, get_player());
	}
	else
	{
		self->state = ES_Idle;
		self->velocity.x = 0;
	}

}

void enemy_touch_regular(Entity *self, Entity *other)
{
	if (other->id == 0)
	{
		gf2d_entity_deal_damage(other, self, self, 10, vector2d(0,0));
		slog("touching player");
	}
}

Entity *spawn_enemy_small(Vector2D position)
{

}

void enemy_seek(Entity *self, Entity *target)
{
	if (target->position.x < self->position.x && self->canmove != 1)
		self->velocity.x = -.5;
	if (target->position.x > self->position.x && self->canmove != -1)
		self->velocity.x = .5;

}

int enemy_damage(Entity *self, int amount, Entity *source)
{
	self->health -= amount;
	if (self->health <= 0)
		self->die(self);
}
void enemy_die(Entity *self)
{
	gf2d_entity_free(self);
}

