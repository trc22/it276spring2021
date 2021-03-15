#include "simple_logger.h"
#include "item.h"

typedef struct
{
	Item *item_list;
	Uint32  max_items;
}Inventory;

static Inventory inventory = { 0 };

void inventory_init(Uint32 max_items)
{
	if (max_items == 0)
	{
		slog("cannot allocate 0 entities!");
		return;
	}
	if (inventory.item_list != NULL)
	{
		inventory_free();
	}
	inventory.item_list = (Item *)gfc_allocate_array(sizeof (Item), max_items);
	if (inventory.item_list == NULL)
	{
		slog("failed to allocate entity list!");
		return;
	}
	inventory.max_items = max_items;
	atexit(inventory_free);
	slog("Inventory initialized");
}

void inventory_update_items();

void inventory_free()
{
	if (inventory.item_list != NULL)
	{
		free(inventory.item_list);
	}
	memset(&inventory, 0, sizeof(inventory));
	slog("inventory freed");
}

Item *item_new()
{
	int i;
	if (inventory.max_items == NULL)
	{
		slog("entity system does not exist");
		return NULL;
	}
	for (i = 0; i < inventory.max_items; i++)
	{
		if (inventory.item_list[i]._inuse)continue;// someone else is using this one
		memset(&inventory.item_list[i], 0, sizeof(Item));
		inventory.item_list[i]._inuse = 1;
		return &inventory.item_list[i];
	}
	return NULL;
}

void item_free(Item *item)
{
	if (!item)
	{
		slog("cannot free a NULL item");
		return;
	}
	item->_inuse = 0;
}

