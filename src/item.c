#include "simple_logger.h"
#include "gf2d_actor.h"
#include "gf2d_font.h"

#include "item.h"

//#include "pickup.h"
//#include "overlay.h"

typedef struct
{
	Item *item_list;
	Uint32  max_items;
}ItemManager;


static ItemManager item_manager = { 0 };
static ItemManager inventory = { 0 };
static Uint32 tetris_inventory[6][8] = { 0 };

Actor inventory_actor;
Sprite *cursor;

Vector3D *rotation;

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

	item_load(0, "", NULL, true, false, false, NULL, 0, -1, vector2d(0, 0), 0);
	item_load(1, "flashlight", "images/inventory/flashlight.png", true, false, false, NULL, -1, -1, vector2d(1, 2), 0);
	item_load(4, "pistol", "images/inventory/pistol.png", false, true, true, NULL, 12, 12, vector2d(2, 2), 0);
	item_load(6, "rifle", "images/inventory/rifle.png", false, true, true, NULL, 4, 4, vector2d(1, 5), 0);
	/*	item_load(true, "pizza", 1, 4, 5, 50, false, 0);
	item_load(true, "key", 3, -1, -1, 25, false, 0);
	
	//Weapons
	item_load(true, "knife", 13, -1, -1, 60, false, 0);
	item_load(true, "shotgun", 5, -1, -1, 90, true, 9);
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
Item *item_load(int id, char *name, char *sprite, Bool usable, Bool canEquip, Bool hasAmmo, int ammoID, int max, int quantity, Vector2D size, int cooldown)
{
	Item *item;

	if (item_manager.item_list[id]._inuse) return NULL;// someone else is using this one
	memset(&item_manager.item_list[id], 0, sizeof(Item));
	item_manager.item_list[id]._inuse = 1;

	item = &item_manager.item_list[id];
	item->itemID = id;
	item->itemName = name;
	if (sprite != NULL && sprite != "")
		item->sprite = gf2d_sprite_load_image(sprite);
	else
		item->sprite = NULL;
	item->rotation = vector3d_new();
	item->rotation->x = 0;
	item->rotation->y = 0;
	item->rotation->z = 0;
	item->_usable = usable;
	item->_equippable = canEquip;
	item->_equipped = 0;
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

void free_item(Item *item)
{
	if (!item)
	{
		slog("cannot free a NULL item");
		return;
	}
	item->_inuse = 0;
	item->itemID = 0;
	item->itemName = "";
}

Item *get_item_by_id(int id)
{
	return &item_manager.item_list[id];
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

void inventory_insert_item(Item *item)
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

void inventory_remove_item(Item *item)
{
	int i;
	for (i = 0; i < inventory.max_items; i++)
	{
		if (inventory.item_list[i].itemID == item->itemID)
		{
			item_remove_tetris(item->itemID);
			free_item(&inventory.item_list[i]);
			return;
		}
	}
}

Item *get_item_by_pos(int pos)
{
	if (pos < 0 || pos > inventory.max_items)
	{
		slog("Inventory pos is out of range");
		return NULL;
	}
	if (inventory.item_list[pos]._inuse)
		return &inventory.item_list[pos];
	return NULL;
}

int get_item_inventory_pos(Item *item)
{
	int i;
	for (i = 0; i < inventory.max_items; i++)
	{
		if (item == &inventory.item_list[i])
			return i;
	}
	return -1;
}

Item *search_inventory(int id)
{
	int i;
	for (i = 0; i < inventory.max_items; i++)
	{
		if (inventory.item_list[i].itemID == id)
			return &inventory.item_list[i];
	}
	return NULL;
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

	gf2d_actor_free(&inventory_actor);
	gf2d_actor_load(&inventory_actor, "actors/tetris_inventory.actor");
	gf2d_actor_set_action(&inventory_actor, "idle");

	cursor = gf2d_sprite_load_image("images/inventory/cursor.png");

	atexit(inventory_free);

}

int item_insert_tetris(Item *item, Vector2D location, int _ismove)
{
	int i, j;
	int row, column;

	row = location.x;
	column = location.y;

	
	if (row > 5 || row < 0)
	{
		slog("tetris row bounds");
		return 0;
	}
	if (column > 7 || column < 0)
	{
		slog("tetris column out of bounds");
		return 0;
	}
	if (&item->itemSize == NULL)
	{
		slog("item size is null");
		return 0;
	}

	if (tetris_inventory[row][column] != 0)
	{
		slog("slot is taken");
		return 0;
	}

	for (i = 0; i < item->itemSize.y; i++)
	{

		for (j = 0; j < item->itemSize.x; j++)
		{
			if (tetris_inventory[row - i][column - j] != 0) //Starting from the right, if x rows are taken;
			{
				slog("slot is taken");
				return 0;
			}
			if (row - i < 0 || column - (item->itemSize.x - 1) < 0)
			{
				slog("can't put an item there");
				return 0;
			}
		}
	}
	for (i = 0; i < item->itemSize.y; i++)
	{
		for (j = 0; j < item->itemSize.x; j++)
		{
			tetris_inventory[row - i][column - j] = item->itemID; //Starting from the right;
		}
	}

	item->pos = location;
	if (!_ismove)
		inventory_insert_item(item);

	slog("Updated inventory");
	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 8; j++)
		{
			printf("%i, ", tetris_inventory[i][j]);
		}
		printf("\n");
	}

	return 1;

}

void item_remove_tetris(int id)
{
	int i, j;
	
	//slog("removing %s:", item->itemName);
	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (tetris_inventory[i][j] == id)
				tetris_inventory[i][j] = 0;
		}
	}
}

Item *item_find_tetris(Vector2D location)
{
	int row, column;
	row = location.x;
	column = location.y;
	
	return get_item_by_id(tetris_inventory[row][column]);
}

int item_move_tetris(Item *item, Vector2D src, Vector2D dst)
{
	int i, j;
	item_remove_tetris(item->itemID);
	if (item_insert_tetris(item, dst, 1))
	{
		slog("moved item");
		vector2d_copy(search_inventory(item->itemID)->pos, dst);
		return 1;
	}
	item_insert_tetris(item, src, 1);
	vector2d_copy(search_inventory(item->itemID)->pos, src);
	slog("failed to move");
	return 0;
}

void item_rotate_tetris(Item *item)
{
	int x, y;

	item->rotation->z += 90;
	if (item->rotation->z == 360)
		item->rotation->z = 0;

	x = item->itemSize.y;
	y = item->itemSize.x;
	vector2d_copy(item->itemSize, vector2d(x,y));

	x = item->pos.y;
	y = item->pos.x;
	vector2d_copy(item->pos, vector2d(x, y));

}

void inventory_free()
{
//	free(tetris_inventory);
}

void draw_inventory()
{
	int x,y,i;
	Item *item;


	gf2d_sprite_draw_image(inventory_actor.sprite, vector2d(425, 100)); //Draw grid

	for (i = 0; i < inventory.max_items; i++)
	{
		item = get_item_by_pos(i);

		if (item == NULL || !item->_inuse || item->itemID == 0) continue;

		//Convert from inventory space to screen space

		x = item->pos.y - (item->itemSize.x - 1);
		y = item->pos.x - 1;
		if (item->itemSize.y > 2)
			y = item->pos.x - (item->itemSize.y - 1);


		if (item->rotation->z > 0)
		{
			if (item->rotation->z == 90)
			{
				x += item->itemSize.x;
				if (item->itemSize.x == 1)
					y += 1;
				if (item->itemSize.y > 2)
					y = item->pos.x;
			}
			else if (item->rotation->z == 180)
			{
				x += item->itemSize.x;
				y += 2;
				if (item->itemSize.y > 2)
					y = item->pos.x + 1;
			}
			else if (item->rotation->z == 270)
			{
				y += 2;
				if (item->itemSize.x == 1)
					x = item->pos.y - (item->itemSize.y - 1);
				if (item->itemSize.y > 2)
				{
					x = item->pos.y - (item->itemSize.y - 1);
					y = item->pos.x + 1;
				}
			}
			
		}
		gf2d_sprite_draw(item->sprite, vector2d((x * 32) + 425, (y * 32) + 100), NULL, NULL, item->rotation, NULL, NULL, 0);
	}
}

void draw_cursor_inventory(Vector2D location)
{
	int x, y;
	//Convert from inventory space to screen space
	x = location.y; 
	y = location.x;

	gf2d_sprite_draw_image(cursor, vector2d((x *32) + 425, (y *32) + 100));
}

