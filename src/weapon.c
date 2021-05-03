#include "weapon.h"
#include "player.h"

int projectile_touch(Entity *self, Entity *other);
void projectile_think(Entity *self);


Entity *weapon_fire_projectile(Vector2D position, Vector2D velocity, int damage)
{
	Entity *ent;
	ent = entity_spawn("actors/projectile.actor", "projectile", position);
	vector2d_copy(ent->velocity, velocity);

	ent->id = 6;
	ent->body.team = 0;
	ent->grounded = 1;
	ent->health = damage;
	ent->touch = projectile_touch;
	ent->think = projectile_think;
	return ent;
}

void weapon_fire_pistol()
{
	Entity *player = get_player();

	if (player->flip.x == 0)
		weapon_fire_projectile(vector2d(player->position.x + 36, player->position.y + 20), vector2d(1, 0), 20);
	else
		weapon_fire_projectile(vector2d(player->position.x - 4, player->position.y + 20), vector2d(-1, 0), 20);
}

int projectile_touch(Entity *self, Entity *other)
{

	gf2d_entity_deal_damage(other, get_player(), get_player, self->health, vector2d(0, 0));
	self->inuse = 0;
}

void projectile_think(Entity *self)
{
	if (self->canmove != 0)
		self->inuse = 0;
}