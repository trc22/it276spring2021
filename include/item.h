#ifndef __ITEM_H__
#define __ITEM_H__

#include "gfc_types.h"
#include "simple_json.h"


typedef struct Item_s
{
	Bool		_inuse;
	Bool		_consumable;
	Bool		_equipped;
	char		*itemName;
	int			itemID;
	int			quantity;
	int			max_quantity;
	int			timer;
	int			timerMax;
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

Item *item_load(Bool usable, char *name, int id, int amount, int max_amount, int useTimer);

Item *get_current_item(int i);

Item *get_item_by_id(int id);

void check_empty(Item *item);

#endif
