#ifndef __ITEM_H__
#define __ITEM_H__

#include "gfc_types.h"
#include "simple_json.h"


typedef struct Item_s
{
	Bool		_inuse;
	Bool		_consumable;
	char		*itemName;
	int			itemID;
	int			quantity;
	int			max_quantity;
	void		(*update)(struct Item_s *self);
	void		(*use)(struct Item_s *self);
	void		(*free)(struct Item_s *self);
	void         *data;
}Item;

void inventory_init(Uint32 max_items);

void inventory_update_items();

void inventory_free();

void inventory_insert(Item *item);

void item_free(Item *item);

void items_free(Item *item);

Item *item_load(Bool usable, char *name, int id, int amount, int max_amount);


#endif
