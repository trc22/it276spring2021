#include "simple_logger.h"
#include "gfc_input.h"

#include "interactables.h"
#include "level.h"

typedef struct
{
	Uint32 maxInteracts;
	Interactable *interactList;
}InteractManager;

static InteractManager interact_manager = { 0 };

int interact_door(Entity *self, Entity *other);
int interact_switch(Entity *self, Entity *other);


void interactable_system_init(Uint32 maxInteracts)
{
	if (maxInteracts == 0)
	{
		slog("cannot allocate 0 entities!");
		return;
	}
	if (interact_manager.interactList != NULL)
	{
		return;
	}
	interact_manager.interactList = (Interactable *)gfc_allocate_array(sizeof (Interactable), maxInteracts);
	if (interact_manager.interactList == NULL)
	{
		slog("failed to allocate Interactable list!");
		return;
	}
	interact_manager.maxInteracts = maxInteracts;
//	atexit(interactable_system_close);
	slog("Interactable system initialized");
}

void interactable_system_close()
{

}

Interactable *interactable_new()
{
	int i;
	if (interact_manager.interactList != NULL)
	{
		for (i = 0; i < interact_manager.maxInteracts; i++)
		{
			if (interact_manager.interactList[i]._inuse) continue;
			interact_manager.interactList[i].id = i;
			return &interact_manager.interactList[i];
		}
	}

	slog("interactable list is full");
	return NULL;
}

Interactable *interactable_spawn(Vector2D location, int interact_type, char *destination, Bool need_key, Vector2D switch_effect)
{
	Interactable *interactable;
	interactable = interactable_new();

	Entity *ent;

	if (interact_type == IT_DOOR)
	{
		ent = entity_spawn("actors/player.actor", "door", location);
		ent->touch = interact_door;
		interactable->destination = destination;
		interactable->require_key = need_key;
	}
	else if (interact_type == IT_SWITCH)
	{
		ent = entity_spawn("actors/player.actor", "switch", location);
		ent->touch = interact_switch;
		interactable->destination = destination;
		interactable->require_key = need_key;
		interactable->switch_effect = switch_effect;
	}
	
	ent->id = 12;
	ent->interact_id = interactable->id;

	interactable->_inuse = 1;

	return interactable;

}


Interactable *interactable_get_by_id(int id)
{
	int i;

	slog("Getting interactable %i", id);
	if (interact_manager.interactList != NULL)
	{
		for (i = 0; i < interact_manager.maxInteracts; i++)
		{
			if (!interact_manager.interactList[i]._inuse) continue;
			
			if (interact_manager.interactList[i].id == id)
				return &interact_manager.interactList[i];
		}
	}
	slog("could not find interactable");
	return NULL;
}

int interact_door(Entity *self, Entity *other)
{
	if (other->id == 0 && gfc_input_command_pressed("interact"))
	{
		slog("Destination: %s", interactable_get_by_id(self->interact_id)->destination);
		level_transition(interactable_get_by_id(self->interact_id)->destination, 0);
	}
	return 1;
}

int interact_switch(Entity *self, Entity *other)
{
	if (other->id == 0 && gfc_input_command_pressed("interact"))
	{
		Interactable *interact; 
		interact = interactable_get_by_id(self->interact_id);
		interact->type = IT_DOOR;
		self->touch = interact_door;
		self->position = interact->switch_effect;
	}
	return 1;
}