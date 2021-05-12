#include "simple_logger.h"

#include "pickup.h"
#include "player.h"


typedef struct
{
	Uint32 maxPickups;
	Pickup *pickupList;
}PickupManager;

static PickupManager pickupManager = { 0 };

int pickup_touch(Entity *self, Entity* other);


void init_pickup_system(Uint32 maxPickups)
{
	if (maxPickups == 0)
	{
		slog("cannot allocate 0 pickups!");
		return;
	}
	if (pickupManager.pickupList != NULL)
	{
		inventory_free();
	}
	pickupManager.pickupList = (Pickup *)gfc_allocate_array(sizeof (Pickup), maxPickups);
	if (pickupManager.pickupList == NULL)
	{
		slog("failed to allocate pickup list!");
		return;
	}
	pickupManager.maxPickups = maxPickups;
	atexit(pickup_system_close);
	slog("Pickup system initialized");
}

void pickup_system_close()
{
	int i;
	if (pickupManager.pickupList != NULL)
	{
		for (i = 0; i < pickupManager.maxPickups; i++)
		{
			pickup_free(&pickupManager.pickupList[i]);
		}
		free(pickupManager.pickupList);
	}
	memset(&pickupManager, 0, sizeof(pickupManager));
	slog("pickup system closed");
}

Pickup *pickup_new()
{
	int i;
	if (pickupManager.pickupList != NULL)
	{
		for (i = 0; i < pickupManager.maxPickups; i++)
		{
			if (pickupManager.pickupList[i]._inuse) continue;
			return &pickupManager.pickupList[i];
		}
	}

	slog("Pickup list is full");
	return NULL;
}

Pickup *pickup_spawn(Vector2D location, char *pickup_name, int pickup_id, char* pickup_sprite, int item_id)
{
	Pickup *pickup;
	pickup = pickup_new();

	vector2d_copy(pickup->position, location);
	pickup->entity = entity_spawn("actors/interacts.actor", pickup_name, location);
	gf2d_actor_set_action(&pickup->entity->actor, "pickup");
	pickup->entity->id = 3;
	pickup->entity->pickup_id = pickup_id;
	pickup->entity->touch = pickup_touch;

	pickup->item = get_item_by_id(item_id);
	pickup->item->pickup = item_id;

	pickup->_inuse;
	pickup->id = pickup_id;
	pickup->name = pickup_name;
	return pickup;
}

Pickup *pickup_get_by_id(int pickup_id)
{
	int i;
	if (pickupManager.pickupList != NULL)
	{
		for (i = 0; i < pickupManager.maxPickups; i++)
		{
			if (pickupManager.pickupList[i].id == pickup_id)
				return &pickupManager.pickupList[i];
		}
	}
	return NULL;
}

void pickup_free(Pickup *pickup)
{
	int i;
	if (pickupManager.pickupList != NULL)
	{
		for (i = 0; i < pickupManager.maxPickups; i++)
		{
			if (!pickupManager.pickupList[i]._inuse) continue;
			if (pickupManager.pickupList[i].id == pickup->id)
			{
				pickup->_inuse;
			}
		}
	}
}

int pickup_touch(Entity *self, Entity* other)
{
	Pickup *pickup;

	if (other->id == 0)
	{
		pickup = pickup_get_by_id(self->pickup_id);

		inventory_insert_item(pickup->item);
		
		player_inventory_insert(pickup->item);

		self->inuse = 0;
	}
	return 1;
}

void pickup_respawn_ent(Pickup *pickup)
{
	pickup->entity = entity_spawn("actors/player.actor", pickup->name, pickup->position);
	pickup->entity->touch = pickup_touch;
	pickup->entity->id = 3;
	pickup->entity->pickup_id = pickup->id;
}

void pickup_insert_fail(Item *item)
{
	Pickup *pickup;
	pickup = pickup_get_by_id(item->pickup);

	pickup_respawn_ent(pickup);
	//pickup->item = get_item_by_id(item->itemID);
	inventory_remove_item(item);
}