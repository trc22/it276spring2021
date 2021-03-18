#include "simple_logger.h"
#include "weapons.h"

void projectile_think(Entity *self);

Entity *fire_projectile(Vector2D origin, Vector2D velocity)
{
	Entity *ent;
	ent = entity_new();
	ent->sprite = gf2d_sprite_load_all("images/bullet.png", 8, 4, 1);
	vector2d_copy(ent->position, origin);
	ent->frameRate = 0.1;
	ent->frameCount = 1;
	ent->think = projectile_think;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	ent->type = 5; //Bullet
	ent->velocity = velocity;
	ent->_touchingWall = false;
	return ent;
}

void projectile_think(Entity *self)
{
	if (!self)return;
	self->velocity.x += (self->velocity.x * .25);
	if (!camera_rect_on_screen(gfc_sdl_rect(self->position.x, self->position.y, self->sprite->frame_w, self->sprite->frame_h)))
		entity_free(self);
}

Entity create_hitbox(Vector2D origin, int duration);

void fire_pistol(Item *pistol, Vector2D player_position, float player_rotation)
{
	Vector2D position;
	position.y = player_position.y + 64;
	if (handle_ammo(pistol))
	{
		slog("firing pistol!");
		if (player_rotation == 90)
		{
			position.x = player_position.x + 128;
			fire_projectile(position, vector2d(1, 0));
		}
		else
		{
			position.x = player_position.x;
			fire_projectile(position, vector2d(-1, 0));
		}
	}
	return;
}

void fire_shotgun(Item *shotgun, Vector2D player_position, float player_rotation)
{
	Vector2D position;
	position.y = player_position.y + 64;
	if (handle_ammo(shotgun))
	{
		slog("firing shotgun!");
		if (player_rotation == 90)
		{
			position.x = player_position.x + 128;
			position.y -= 20;
			fire_projectile(position, vector2d(1, 0))->rotation.z = 120;
			position.y += 10;
			fire_projectile(position, vector2d(1, 0));
			position.y += 10;
			fire_projectile(position, vector2d(1, 0));
			position.y += 20;
			fire_projectile(position, vector2d(1, 0))->rotation.z = 60;

		}
		else
		{
			position.x = player_position.x - 64;
			position.y -= 20;
			fire_projectile(position, vector2d(-1, 0))->rotation.z = 120;
			position.y += 10;
			fire_projectile(position, vector2d(-1, 0));
			position.y += 10;
			fire_projectile(position, vector2d(-1, 0));
			position.y += 20;
			fire_projectile(position, vector2d(-1, 0))->rotation.z = 60;
		}
	}
	return;
}

void fire_rifle(Item *rifle, Vector2D player_position, float player_rotation)
{
	Vector2D position;
	position.y = player_position.y + 64;
	if (handle_ammo(rifle))
	{
		slog("firing rifle!");
		if (player_rotation == 90)
		{
			position.x = player_position.x + 128;
			fire_projectile(position, vector2d(1, 0));
		}
		else
		{
			position.x = player_position.x;
			fire_projectile(position, vector2d(-1, 0));
		}
	}
	return;
}