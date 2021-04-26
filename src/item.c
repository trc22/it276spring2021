#include "simple_logger.h"
#include "item.h"
//#include "pickup.h"
//#include "overlay.h"

typedef struct
{
	Item *item_list;
	Uint32  max_items;
}ItemManager;


static ItemManager item_manager = { 0 };
static Uint32 tetris_inventory[6][8] = { 0 };

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

	item_load(0, "", true, false, NULL, 0, -1, vector2d(0, 0), 0);
	item_load(1, "flashlight", true, false, NULL, 0, -1, vector2d(2, 1), 0);
	item_load(4, "pistol", true, true, NULL, 0, -1, vector2d(2, 2), 0);
/*	item_load(true, "pizza", 1, 4, 5, 50, false, 0);
	item_load(true, "key", 3, -1, -1, 25, false, 0);
	
	//Weapons
	item_load(true, "knife", 13, -1, -1, 60, false, 0);
	item_load(true, "pistol", 4, -1, -1, 40, true, 8);
	item_load(true, "shotgun", 5, -1, -1, 90, true, 9);
	item_load(true, "rifle", 6, -1, -1, 120, true, 10);
	item_load(true, "dynamite", 7, 2, 4, 120, false, 0);

	//Ammo
	item_load(false, "pistol ammo", 8, 12, 36, -1, false, 0);
	item_load(false, "shotgun ammo", 9, 4, 16, -1, false, 0);
	item_load(false, "rifle ammo", 10, 4, 10, -1, false, 0);

	//tools
	item_load(true, "flashlight", 2, -1, -1, 25, false, 0);
	item_load(true, "climbing gear", 11, -1, -1, 100, false, 0);
	item_load(true, "lighter", 12, 6, 6, 25, false, 0);
	item_load(true, "tape recorder", 14, 1, 4, 200, false, 0);*/

}
Item *item_load(int id, char *name, Bool usable, Bool hasAmmo, int ammoID, int max, int quantity, Vector2D size, int cooldown)
{
	Item *item;

	if (item_manager.item_list[id]._inuse) return NULL;// someone else is using this one
	memset(&item_manager.item_list[id], 0, sizeof(Item));
	item_manager.item_list[id]._inuse = 1;

	item = &item_manager.item_list[id];

	item->itemID = id;
	item->itemName = name;
	item->_usable = usable;
	item->_hasAmmo = hasAmmo;
	item->ammoID = ammoID;
	item->max_quantity = max;
	item->quantity = quantity;
	item->itemSize = size;
	item->timerMax = cooldown;

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

Item *get_item_by_id(int id)
{
	return &item_manager.item_list[id];
}

void init_inventory_tetris()
{
	int i, j;
	slog("inventory:");
	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 8; j++)
		{
			tetris_inventory[i][j] = 0;
			printf("%i, ", tetris_inventory[i][j]);
		}
		printf("\n");
	}
	atexit(inventory_free);

}

void item_insert_tetris(Item *item, Vector2D location)
{
	int i, j;
	int row, column;

	row = location.x;
	column = location.y;

	
	if (row > 6 || row < 0)
	{
		slog("tetris row bounds");
		return;
	}
	if (column > 8 || column < 0)
	{
		slog("tetris column out of bounds");
		return;
	}
	if (&item->itemSize == NULL)
	{
		slog("item size is null");
		return;
	}

	if (tetris_inventory[row][column] != 0)
	{
		slog("slot is taken");
		return;
	}
	
	if (item->rotate90)
	{
		vector2d_copy(item->itemSize, vector2d(item->itemSize.y, item->itemSize.x));
	}
	for (i = 0; i < item->itemSize.y; i++)
	{

		for (j = 0; j < item->itemSize.x; j++)
		{
			if (tetris_inventory[row - i][column - j] != 0 || column - j < 0) //Starting from the right, if x rows are taken;
			{
				slog("slot is taken");
				return;
			}
		}
	}
	for (i = 0; i < item->itemSize.y; i++)
	{
		for (j = 0; j < item->itemSize.x; j++)
		{
			tetris_inventory[row - i][column - j] = 1; //Starting from the right, if x rows are taken;
		}
	}
	slog("Inserted new item");
	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 8; j++)
		{
			printf("%i, ", tetris_inventory[i][j]);
		}
		printf("\n");
	}

}

void inventory_free()
{
//	free(tetris_inventory);
}