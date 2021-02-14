#include "simple_logger.h"

#include "player.h"

void player_update(Entity *self);

Entity *player_spawn(Vector2D position)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the player");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/ed210_top.png", 128, 128, 16);
	vector2d_copy(ent->position, position);
	ent->frameRate = 0.1;
	ent->frameCount = 16;
	ent->update = player_update;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	return ent;
}


void player_update(Entity *self)
{
	Vector2D aimdir;
	float angle;
	int mx, my;
	if (!self)return;
	SDL_GetMouseState(&mx, &my);
	aimdir.x = mx - (self->position.x + 64);
	aimdir.y = my - (self->position.y + 64);
	angle = vector_angle(aimdir.x, aimdir.y);
	self->rotation.z = angle + 90;
}

/**/
