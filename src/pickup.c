#include "simple_logger.h"
#include "pickup.h"

Entity *spawn_pickup(Vector2D position, int id)
{
	Entity *ent;
	ent = entity_new();

	if (!ent)
	{
		slog("failed to create entity for the pickup");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/pickup.png", 32, 32, 16);
	vector2d_copy(ent->position, position);

	ent->type = 11;
	ent->itemID = id;
	ent->itemQuantity = NULL;
	ent->_canCollide = true;

	return ent;
}