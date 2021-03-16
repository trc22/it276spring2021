#include "simple_logger.h"
#include "item.h"

typedef struct
{
	Item *item_list;
	Uint32  max_items;
}Inventory;

static Inventory item_manager = { 0 };
static Inventory inventory = { 0 };

void load_all_items(Uint32 max_items)
{
	if (max_items == 0)
	{
		slog("cannot allocate 0 entities!");
		return;
	}
	if (item_manager.item_list != NULL)
	{
		inventory_free();
	}
	item_manager.item_list = (Item *)gfc_allocate_array(sizeof (Item), max_items);
	if (item_manager.item_list == NULL)
	{
		slog("failed to allocate entity list!");
		return;
	}
	item_manager.max_items = max_items;
	atexit(items_free);
	slog("Item list initialized");

	item_load(true, "", 0, 0, 0, 0);
	item_load(true, "pizza", 1, 4, 5, 50);
	item_load(true, "flashlight", 2, -1, -1, 25);

}

void inventory_init(Uint32 max_items)
{
	if (max_items == 0)
	{
		slog("cannot allocate 0 items!");
		return;
	}
	if (inventory.item_list != NULL)
	{
		inventory_free();
	}
	inventory.item_list = (Item *)gfc_allocate_array(sizeof (Item), max_items);
	if (inventory.item_list == NULL)
	{
		slog("failed to allocate item list!");
		return;
	}
	inventory.max_items = max_items;
	atexit(inventory_free);
	slog("Inventory initialized");

}

void items_free()
{
	if (item_manager.item_list != NULL)
	{
		free(item_manager.item_list);
	}
	memset(&item_manager, 0, sizeof(item_manager));
	slog("item list freed");
}

void inventory_free()
{
	if (inventory.item_list != NULL)
	{
		free(inventory.item_list);
	}
	memset(&inventory, 0, sizeof(inventory));
	slog("inventory freed");
}

Item *item_load(Bool usable, char *name, int id, int amount, int max_amount, int useTimer)
{
	Item *item;

	if (item_manager.item_list[id]._inuse) return NULL;// someone else is using this one
	memset(&item_manager.item_list[id], 0, sizeof(Item));
	item_manager.item_list[id]._inuse = 1;
	item = &item_manager.item_list[id];

	item->_consumable = usable;
	item->itemName = name;
	item->itemID = id;
	item->quantity = amount;
	item->max_quantity = max_amount;
	item->timer = useTimer;
	item->timerMax = useTimer;

	slog("loading %s, id: %i", item->itemName, item->itemID);
	return item;
}

void inventory_insert(Item *item)
{
	int i;
	if (inventory.max_items == NULL)
	{
		slog("inventory does not exist");
		return;
	}
	if (item == NULL || item->itemName == NULL)
	{
		slog("item is not valid, cannot insert");
		return;
	}

	for (i = 0; i < inventory.max_items; i++)
	{
		if (item != NULL && item->itemID == inventory.item_list[i].itemID)
		{
			inventory.item_list[i].quantity += item->quantity;
			slog("%s already in inventory, updating quantity", item->itemName);
			return;
		}

		if (inventory.item_list[i]._inuse)
			continue;// someone else is using this one

		inventory.item_list[i] = item_manager.item_list[item->itemID];
		inventory.item_list[i]._inuse = 1;
		slog("inserting %s into slot %i", inventory.item_list[i].itemName, i);
		return;
	}
	slog("failed to insert: %s", item->itemName);
	return;
}

void item_free(Item *item)
{
	if (!item)
	{
		slog("cannot free a NULL item");
		return;
	}
	item->_inuse = 0;
	item->itemID = NULL;
	item->itemName = NULL;
}

Item *get_current_item(int i)
{
	return &inventory.item_list[i];
}

Item *get_item_by_id(int id)
{
	return &item_manager.item_list[id];
}

void check_empty(Item *item)
{
	if (item->quantity == 0 || item == NULL)
	{
		slog("Ran out of: %s", item->itemName);
		item_free(item); 
	}
}

void update_timers()
{
	int i;
	Item *item;
	for (i = 0; i < inventory.max_items; i++)
	{
		item = &inventory.item_list[i];
		if (!item->_inuse || item->itemID == NULL)
			continue;
		if (item->timer != item->timerMax)
			item->timer++;
	}
}

