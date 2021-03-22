#ifndef __ITEM_H__
#define __ITEM_H__

#include "gfc_types.h"
#include "simple_json.h"

#include "entity.h"


typedef struct Item_s
{
	Bool		_inuse;
	Bool		_consumable;
	Bool		_equipped;
	Bool		_hasAmmo;
	char		*itemName;
	int			itemID;
	int			quantity;
	int			max_quantity;
	int			timer;
	int			timerMax;
	int			ammoID;
	void		(*free)(struct Item_s *self);
	void         *data;
}Item;

void load_all_items(Uint32 max_items);

void inventory_init(Uint32 max_items);

void inventory_update_items();

void inventory_free();

void inventory_insert(Item *item);

void item_free(Item *item);

void items_free(Item *item);

Item *item_load(Bool usable, char *name, int id, int amount, int max_amount, int useTimer, Bool ammo, int ammoID);

Item *get_current_item(int i);

Item *get_item_by_id(int id);

Item *search_inventory(int id);

Bool handle_ammo(Item *item);

void check_empty(Item *item);

void drop_item(Item *item, Vector2D position);

void clear_inventory();

Bool check_inventory(int id);


#endif
