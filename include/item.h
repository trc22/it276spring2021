#ifndef __ITEM_H__
#define __ITEM_H__

#include "gfc_types.h"

typedef struct Item_s
{
	Bool		_inuse;
	char		*itemName;
	int			itemID;
	int			quantity;
	void		(*update)(struct Item_s *self);
	void		(*free)(struct Item_s *self);
	void         *data;
}Item;

void inventory_init(Uint32 max_items);

void inventory_update_items();

void inventory_free();

Item *item_new();

void item_free(Item *item);

#endif
