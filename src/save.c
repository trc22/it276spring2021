#include "simple_logger.h"
#include "simple_json.h"

#include "save.h"

#include "item.h"
#include "level.h"
#include "player.h"


void save_new(char *current_level, Vector2D position)
{
	SJson *json, *inventory, *inventory_tetris, *row, *item_id_tetris;
	SJson *items, *item_id, *quantitiy, *quantities, *item_positions, *item_pos, *rotations, *rotation;
	SJson *level, *level_name, *player_pos, *x, *y;
	Item *item;
	int i, j;

	slog("creating new save");

	json = sj_object_new();
	inventory = sj_object_new();
	level = sj_object_new();

	inventory_tetris = sj_array_new();
	for (i = 0; i < 6; i++)
	{
		row = sj_array_new();
		for (j = 0; j < 8; j++)
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
	item_positions = sj_array_new();
	rotations = sj_array_new();
	for (i = 0; i < item_get_max_items(); i++)
	{
		item = get_item_by_pos(i);
		if (item == NULL || !item->_inuse || item->itemID < 1)
			continue;

		item_id = sj_new_int(item->itemID);
		quantitiy = sj_new_int(item->quantity);
		sj_array_append(items, item_id);
		sj_array_append(quantities, quantitiy);

		item_pos = sj_array_new();
		x = sj_new_int(item->pos.x);
		y = sj_new_int(item->pos.y);
		sj_array_append(item_pos, x);
		sj_array_append(item_pos, y);
		sj_array_append(item_positions, item_pos);

		rotation = sj_new_int((int)item->rotation->z);
		sj_array_append(rotations, rotation);
	}

	sj_object_insert(inventory, "items", items);
	sj_object_insert(inventory, "quantities", quantities);
	sj_object_insert(inventory, "itemPositions", item_positions);
	sj_object_insert(inventory, "zRotation", rotations);

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

int save_load()
{
	SJson *json, *inventory, *inventory_tetris, *row, *item_id_tetris, *items, *item_id, *quantities;
	SJson *item_positions, *item_pos, *rotations;
	SJson *level, *level_name, *player_pos;

	char *target_level;
	int i, j, x, y;
	int itemID, quantity, rotation;

	x = 0;
	y = 0;
	itemID = 0;
	quantity = 0;
	rotation = 0;

	slog("loading save");
	json = sj_load("saves/save.json");

	level = sj_object_get_value(json, "level");
	level_name = sj_object_get_value(level, "levelName");
	player_pos = sj_object_get_value(level, "playerPos");

	sj_array_get_nth(player_pos, 1);
	
	sj_get_integer_value(sj_array_get_nth(player_pos, 0), &x);
	sj_get_integer_value(sj_array_get_nth(player_pos, 1), &y);
	
	target_level = sj_get_string_value(level_name);

	slog("%s", target_level);
	slog("%i, %i", x, y);

	level_transition(target_level, 0);
	vector2d_copy(get_player()->position, vector2d(x, y));

	inventory = sj_object_get_value(json, "masterInventory");
	inventory_tetris = sj_object_get_value(inventory, "tetrisInventory");
	
	item_clear_all_tetris();
	for (i = 0; i < 6; i++)
	{
		row = sj_array_get_nth(inventory_tetris, i);
		for (j = 0; j < 8; j++)
		{
			sj_get_integer_value(sj_array_get_nth(row, j), &itemID);
			tetris_load(itemID, i, j);
		}
	}

	items = sj_object_get_value(inventory, "items");
	quantities = sj_object_get_value(inventory, "quantities");
	item_positions = sj_object_get_value(inventory, "itemPositions");
	rotations = sj_object_get_value(inventory, "zRotation");
	for (i = 0; i < sj_array_get_count(items); i++)
	{
		sj_get_integer_value(sj_array_get_nth(items, i), &itemID);
		sj_get_integer_value(sj_array_get_nth(quantities, i), &quantity);
		sj_get_integer_value(sj_array_get_nth(rotations, i), &rotation);
		slog("rotation: %i", rotation);
		item_pos = sj_array_get_nth(item_positions, i);
		sj_get_integer_value(sj_array_get_nth(item_pos, i), &x);
		sj_get_integer_value(sj_array_get_nth(item_pos, i), &y);

		inventory_load(itemID, quantity, vector2d(x, y), rotation);
	}
	
	return 1;
}
