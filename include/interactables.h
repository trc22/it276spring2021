#ifndef __INTERACTABLES_H__
#define __INTERACTABLES_H__

#include "gf2d_entity.h"

typedef enum
{
	IT_DOOR,
	IT_SWITCH,
	IT_BOX,
	IT_SAVE
}InteractableType;

typedef struct Interactable_s
{
	Bool		_inuse;
	char		*name;
	int			id;
	int			type;
	char		*destination;
	Bool		require_key;
	Vector2D	switch_effect;
	void		*data;
}Interactable;

void interactable_system_init(Uint32 maxInteracts);

void interactable_system_close();

Interactable *interactable_new();

Interactable *interactable_spawn(Vector2D location, int interact_type, char *destination, Bool need_key, Vector2D switch_effect);

Interactable *interactable_get_by_id(int id);


#endif
