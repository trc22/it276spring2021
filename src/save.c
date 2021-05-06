#include "simple_logger.h"
#include "simple_json.h"

#include "save.h"

#include "item.h"
#include "level.h"

void save_new(char *current_level, Vector2D position)
{
	SJson *json, *inventory,*inventory_tetris, *row, *item_id_tetris, *items, *item_id, *quantitiy, *quantities;
	SJson *level, *level_name, *player_pos, *x, *y;
	Item *item;
	int i, j;

	slog("creating new save");

	json = sj_object_new();
	inventory = sj_object_new();
	level = sj_object_new();

	inventory_tetris = sj_array_new();
	for (i = 0; i < 5; i++)
	{
		row = sj_array_new();
		for (j = 0; j < 7; j++)
		{
			item_id_tetris = sj_new_int(item_get_id_tetris(vector2d(i, j)));
			sj_array_append(row, item_id_tetris);
		}
		sj_array_append(inventory_tetris, row);
	}

	sj_object_insert(inventory, "tetrisInventory", inventory_tetris);
	slog("tetris inventory saved");

	items = sj_array_new();
	quantities = sj_array_new();
	for (i = 0; i < item_get_max_items(); i++)
	{
		item = get_item_by_pos(i);
		if (item == NULL || !item->_inuse || item->itemID < 1)
			continue;
		item_id = sj_new_int(item->itemID);
		quantitiy = sj_new_int(item->quantity);
		sj_array_append(items, item_id);
		sj_array_append(quantities, quantitiy);
	}

	sj_object_insert(inventory, "items", items);
	sj_object_insert(inventory, "quantities", quantities);

	slog("player items saved");

	sj_object_insert(json, "masterInventory", inventory);

	slog("inventory saved");

	level_name = sj_new_str(current_level);
	player_pos = sj_array_new();
	x = sj_new_int(position.x);
	y = sj_new_int(position.y);
	sj_array_append(player_pos, x);
	sj_array_append(player_pos, y);

	sj_object_insert(level, "levelName", level_name);
	sj_object_insert(level, "playerPos", player_pos);
	sj_object_insert(json, "level", level);

	sj_save(json, "saves/save.json");
	
	slog("save written");

}