#include "simple_logger.h"

#include "weapon.h"
#include "player.h"
#include "camera.h"


int projectile_touch(Entity *self, Entity *other);
void projectile_think(Entity *self);

void weapon_particles(Vector2D position, Vector2D velocity, Vector2D variance);

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
	Vector2D position;
	Vector2D velocity;
	Vector2D variance;

	int offset;

	if (player->flip.x == 0)
	{
		offset = 36;
		position = vector2d(player->position.x + 36, player->position.y + 20);
		velocity = vector2d(3, 0);
		variance = vector2d(1, 0);
	}
			
	else if (player->flip.x == -1)
	{
		offset = -4;
		position = vector2d(player->position.x - 4, player->position.y + 20);
		velocity = vector2d(-3, 0);
		variance = vector2d(-1, 0);
	}

	weapon_fire_projectile(position, velocity, 20);
	
	position.x -= (offset + 4);

	weapon_particles(position, velocity, variance);
}

void weapon_fire_rifle()
{
	Entity *player = get_player();
	Vector2D position;
	Vector2D velocity;
	Vector2D variance;

	int offset;

	if (player->flip.x == 0)
	{
		offset = 36;
		position = vector2d(player->position.x + 36, player->position.y + 20);
		velocity = vector2d(4, 0);
		variance = vector2d(1, 0);
	}

	else if (player->flip.x == -1)
	{
		offset = -4;
		position = vector2d(player->position.x - 4, player->position.y + 20);
		velocity = vector2d(-4, 0);
		variance = vector2d(-1, 0);
	}

	weapon_fire_projectile(position, velocity, 100);

	position.x -= (offset + 4);

	weapon_particles(position, velocity, variance);

}

int projectile_touch(Entity *self, Entity *other)
{

	gf2d_entity_deal_damage(other, get_player(), get_player, self->health, vector2d(0, 0));
	self->inuse = 0;
	return 1;
}

void projectile_think(Entity *self)
{
	if (self->canmove != 0)
		self->inuse = 0;
	if (!camera_point_on_screen(self->position))
		self->inuse = 0;
}

void weapon_init_particles(char *sprite)
{

}

void weapon_particles(Vector2D position, Vector2D velocity, Vector2D variance)
{
	Entity *player;
	player = get_player();

	vector2d_add(position, camera_get_offset(), position);
	particle_new_position(player->pe, position);
	particle_new_velocity(player->pe, velocity, variance);
	gf2d_particle_new_default(player->pe, 100);
	//slog("spawning particles");

	
}