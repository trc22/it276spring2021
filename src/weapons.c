#include "simple_logger.h"
#include "weapons.h"

void projectile_think(Entity *self);
void hitbox_think (Entity *self);
void throw_think(Entity *self);

void projectile_collide(Entity *self, Entity *other);
void hitbox_collide(Entity *self, Entity *other);
void throw_collide(Entity *self, Entity *other);


Entity *fire_projectile(Vector2D origin, Vector2D velocity)
{
	Entity *ent;
	ent = entity_new();
	ent->sprite = gf2d_sprite_load_all("images/bullet.png", 8, 4, 1);
	vector2d_copy(ent->position, origin);
	ent->frameRate = 0.1;
	ent->frameCount = 1;
	ent->think = projectile_think;
	ent->collide = projectile_collide;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	ent->type = 5; //Bullet
	ent->velocity = velocity;
	ent->_touchingWall = false;
	ent->_canCollide = true;
	return ent;
}

void projectile_think(Entity *self)
{
	if (!self)return;
	self->velocity.x += (self->velocity.x * .25);
	if (!camera_rect_on_screen(gfc_sdl_rect(self->position.x, self->position.y, self->sprite->frame_w, self->sprite->frame_h)))
		entity_free(self);
}

void projectile_collide(Entity *self, Entity *other)
{
	if (other->type == 2)
	{
		entity_damage(other, 10);
		entity_free(self);
	}
}

Entity *create_hitbox(Vector2D origin, Vector2D size, int duration)
{
	Entity *ent;
	ent = entity_new();
	ent->sprite = gf2d_sprite_load_all("images/hitbox_visible.png", size.x, size.y, 1);
	vector2d_copy(ent->position, origin);
	ent->frameRate = 0.1;
	ent->frameCount = 1;
	ent->think = hitbox_think;
	ent->collide = hitbox_collide;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	ent->type = 6; //hitbox
	ent->_touchingWall = false;
	ent->duration = duration;
	ent->_canCollide = true;
	return ent;
}

void hitbox_think(Entity *self)
{
	if (self->duration == 0)
		entity_free(self);
	else
		self->duration--;
}

void hitbox_collide(Entity *self, Entity *other)
{
	slog("collide");
}

Entity *throw_dynamite(Vector2D origin, Vector2D velocity)
{
	Entity *ent;
	ent = entity_new();
	ent->sprite = gf2d_sprite_load_all("images/bullet.png", 8, 4, 1);
	vector2d_copy(ent->position, origin);
	ent->frameRate = 0.1;
	ent->frameCount = 1;
	ent->think = throw_think;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	ent->type = 7; //dynamite
	ent->velocity = velocity;
	ent->_touchingWall = false;
	ent->duration = 100;
	return ent;
}

void throw_think(Entity *self)
{
	self->velocity.x += self->velocity.x * 0.025;
	if (self->duration > 50)
		self->velocity.y -= .025;
	else
		self->velocity.y += 0.075;

	if (self->duration == 0)
	{
		self->position.x -= 128;
		self->position.y -= 128;
		create_hitbox(self->position, vector2d(256, 256), 10)->type = 8; //create exploision
		entity_free(self);
	}
	else
		self->duration--;
}


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
			position.x = player_position.x;
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

void fire_knife(Item *knife, Vector2D player_position, float player_rotation)
{
	Vector2D position;
	position.y = player_position.y + 32;
	if (player_rotation == 90)
		position.x = player_position.x + 96;
	else
		position.x = player_position.x - 32;
	create_hitbox(position, vector2d(64, 64), 10);
	return;
}

void fire_dynamite(Item *dynamite, Vector2D player_position, float player_rotation)
{
	Vector2D position;
	position.y = player_position.y + 32;
	if (player_rotation == 90)
	{
		position.x = player_position.x + 96;
		throw_dynamite(position, vector2d(1, 0));
	}
	else
	{
		position.x = player_position.x - 32;
		throw_dynamite(position, vector2d(-1, 0));
	}
	return;
}


