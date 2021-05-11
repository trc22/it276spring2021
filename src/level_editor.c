#include "simple_logger.h"
#include "simple_json.h"
#include "windows_common.h"
#include "gf2d_element_entry.h"
#include "gfc_input.h"
#include "gf2d_font.h"


#include "level_editor.h"

Level *current_level;

SJson *json, *levelJS, *levelMap, *row, *level_array, *tile, *tileSet, *tileDimensions, *tileFPL;
SJson *player_start, *player_x, *player_y, *enemies, *enemy_spawns, *enemy_spawn, *enemy, *enemy_x, *enemy_y;
SJson *items, *item_spawn, *item_spawns, *item, *item_x, *item_y;
SJson *interactables, *interact, *interact_spawns, *interact_spawn, *interact_x, *interact_y, *has_key, *key, *switch_effects, *effect;

int status, pos, count, count_real;

int level_length, level_width, enemy_count, pickup_count, interact_count, interact_type;

static Window *_textEntry = NULL;
static Window *_editor = NULL;
static Window *_yesNo = NULL;

List textList;

char count_text[100];

int inputTimer;

void new_yes_no_window(char * text);
void level_editor_reset();

void onYes(void *data)
{
	int i, j;

	pos++;
	switch (status)
	{
	case LE_TILES:

		if (pos == 0)
		{
			level_length = count_real;
			slog("%i", level_length);
			new_yes_no_window("Level width");
			return;
		}
		else if (pos == 1)
		{
			level_width = count_real;
			slog("%i", count);
			new_yes_no_window("Confirm?");
			return;
		}
		else if (pos == 2)
		{
			level_array = sj_array_new();
			for (i = 0; i < level_length; i++)
			{
				row = sj_array_new();
				for (j = 0; j < level_width; j++)
				{
					if (i == level_length - 1)
					{
						tile = sj_new_int(1);
						sj_array_append(row, tile);
						continue;
					}
					if (j == 0 || j == level_width - 1)
					{
						tile = sj_new_int(1);
						sj_array_append(row, tile);
						continue;
					}
					tile = sj_new_int(0);
					sj_array_append(row, tile);
				}
				sj_array_append(level_array, row);
			}
			onYes(NULL);
		}
		else if (pos == 3)
		{
			new_yes_no_window("Tile type (1-5)");
		}
		else if (pos == 4)
		{
			if (count_real < 1 || count_real > 5)
			{
				pos -= 2;
				onYes(NULL);
			}
			else
			{
				tileSet = sj_new_str("images/basetileset.png");
				tileFPL = sj_new_int(1);
				new_yes_no_window("Tile width/length");
			}
			return;
		}
		else if (pos == 5)
		{
			tileDimensions = sj_new_int(count_real);
			level_editor_reset();
			return;
		}
	case LE_PLAYER:
		if (pos == 0)
		{
			player_x = sj_new_int(count_real);
			new_yes_no_window("Player y value");
			return;
		}
		else if (pos == 1)
		{
			player_y = sj_new_int(count_real);
			new_yes_no_window("Confirm?");
		}
		else if (pos == 2)
		{
			player_start = sj_array_new();
			sj_array_append(player_start, player_x);
			sj_array_append(player_start, player_y);
			level_editor_reset();
		}
		return;
	case LE_ENEMY:
		if (pos == 0)
		{
			enemy_count = count_real;
			if (enemy_count <= 0)
			{
				level_editor_reset();
				return;
			}
			enemies = sj_array_new();
			enemy_spawns = sj_array_new();
			new_yes_no_window("Continue?");
		}
		else if (pos == 1)
		{
			if (enemy_count == 0)
			{
				level_editor_reset();
				return;
			}
			new_yes_no_window("Enemy type (1-5)?");
		}
		else if (pos == 2)
		{
			enemy = sj_new_int(count_real);
			sj_array_append(enemies, enemy);
			new_yes_no_window("Enemy x pos:");
		}
		else if (pos == 3)
		{
			enemy_x = sj_new_int(count_real);
			new_yes_no_window("Enemy y pos:");
		}
		else if (pos == 4)
		{
			enemy_y = sj_new_int(count_real);
			new_yes_no_window("Confirm?");
		}
		else if (pos == 5)
		{
			enemy_spawn = sj_array_new();
			sj_array_append(enemy_spawn, enemy_x);
			sj_array_append(enemy_spawn, enemy_y);
			sj_array_append(enemy_spawns, enemy_spawn);

			enemy_count--;
			pos = 0;
			onYes(NULL);
		}
		return;

	case LE_PICKUP:
		if (pos == 0)
		{
			pickup_count = count_real;
			if (pickup_count <= 0)
			{
				level_editor_reset();
				return;
			}
			items = sj_array_new();
			item_spawns = sj_array_new();
			new_yes_no_window("Continue?");
		}
		else if (pos == 1)
		{
			if (pickup_count == 0)
			{
				level_editor_reset();
				return;
			}
			new_yes_no_window("Item ID (1-14)?");
		}
		else if (pos == 2)
		{
			item = sj_new_int(count_real);
			sj_array_append(items, item);
			new_yes_no_window("pickup x pos:");
		}
		else if (pos == 3)
		{
			item_x = sj_new_int(count_real);
			new_yes_no_window("pickup y pos:");
		}
		else if (pos == 4)
		{
			item_y = sj_new_int(count_real);
			new_yes_no_window("Confirm?");
		}
		else if (pos == 5)
		{
			item_spawn = sj_array_new();
			sj_array_append(item_spawn, item_x);
			sj_array_append(item_spawn, item_y);
			sj_array_append(item_spawns, item_spawn);

			pickup_count--;
			pos = 0;
			onYes(NULL);
		}
		return;
	case LE_INTERACT:
		if (pos == 0)
		{
			interact_count = count_real;
			if (interact_count <= 0)
			{
				level_editor_reset();
				return;
			}
			interactables = sj_array_new();
			interact_spawns = sj_array_new();
			switch_effects = sj_array_new();
			has_key = sj_array_new();
			new_yes_no_window("Continue?");
		}
		else if (pos == 1)
		{
			if (interact_count == 0)
			{
				level_editor_reset();
				return;
			}
			new_yes_no_window("Interact type (1-5)?");
		}
		else if (pos == 2)
		{
			interact = sj_new_int(count_real);
			interact_type = count_real;
			sj_array_append(interactables, interact);
			new_yes_no_window("interact x pos:");
		}
		else if (pos == 3)
		{
			interact_x = sj_new_int(count_real);
			new_yes_no_window("interact y pos:");
		}
		else if (pos == 4)
		{
			interact_y = sj_new_int(count_real);
			new_yes_no_window("Confirm?");
		}
		else if (pos == 5)
		{
			interact_spawn = sj_array_new();
			sj_array_append(interact_spawn, interact_x);
			sj_array_append(interact_spawn, interact_y);
			sj_array_append(interact_spawns, interact_spawn);

			new_yes_no_window("Has key? (0-1)");
		}
		else if (pos == 6)
		{
			key = sj_new_int(count_real);
			sj_array_append(has_key, key);
			if (interact_type != 1)
			{
				onYes(NULL);
			}
			new_yes_no_window("Switch effect x:");
		}
		else if (pos == 7)
		{
			if (interact_type != 1)
			{
				interact_x = sj_new_int(0);
				onYes(NULL);
			}
			interact_x = sj_new_int(count_real);
			new_yes_no_window("Switch effect y:");
			
		}
		else if (pos == 8)
		{
			if (interact_type == 1)
			{
				interact_y = sj_new_int(count_real);
			}
			else
			{
				interact_y = sj_new_int(0);
			}
			interact_spawn = sj_array_new();
			sj_array_append(interact_spawn, interact_x);
			sj_array_append(interact_spawn, interact_y);
			sj_array_append(switch_effects, interact_spawn);

			interact_count--;
			pos = 0;
			onYes(NULL);
		}
		return;
	case LE_TEST:
		if (pos == 0)
		{
			level_editor_make();
			return;
		}
		default:
			return;
	}
}

void onNo(void *data)
{
	count -= 2;
	onYes(NULL);
}

void onTile(void *data)
{
	slog("tile");
	_editor = NULL;
	status = LE_TILES;
	pos = -1;
	count = 1;
	count_real = 1;
	//_textEntry = window_text_entry("Level length:", "0", 5000, onYes, onNo);
	_yesNo = window_yes_no("Level length", onYes, onNo, NULL, NULL);
}

void onPlayer(void *data)
{
	slog("player");
	_editor = NULL;
	status = LE_PLAYER;
	pos = -1;
	count = 1;
	count_real = 1;
	_yesNo = window_yes_no("Player x val:", onYes, onNo, NULL, NULL);
}

void onEnemy(void *data)
{
	slog("enemy");
	_editor = NULL;
	status = LE_ENEMY;
	pos = -1;
	count = 1;
	count_real = 1;
	_yesNo = window_yes_no("Enemy amount:", onYes, onNo, NULL, NULL);

}
void onPickup(void *data)
{
	slog("pickup");
	_editor = NULL;
	status = LE_PICKUP;
	pos = -1;
	count = 1;
	count_real = 1;
	_yesNo = window_yes_no("Pickup amount:", onYes, onNo, NULL, NULL);
}

void onInteract(void *data)
{
	slog("interact");
	_editor = NULL;
	status = LE_INTERACT;
	pos = -1;
	count = 1;
	count_real = 1;
	_yesNo = window_yes_no("Interact amount:", onYes, onNo, NULL, NULL);
}

void onBackground(void *data)
{
	slog("background");
	_editor = NULL;
}

void onTest(void *data)
{
	slog("testing");
	_editor = NULL;
	status = LE_TEST;
	pos = -1;
	count = 1;
	count_real = 1;
	new_yes_no_window("Are you sure?");

}

Level * level_editor_init()
{
	Level *level;
	level = level_new();

	slog("creating new save");
	json = sj_object_new();
	levelJS = sj_object_new();
	status = 0;
	count = 0;
	count_real = 0;
	sprintf(count_text, "%d", count);

	_editor = window_level_editor("Level Editor:", onTile, onPlayer, onEnemy, onPickup, onInteract, onBackground, onTest, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	inputTimer = 10;
	return level;
}

Level * level_editor_make()
{
	slog("making level");
	//Background
	//Tiles
	sj_object_insert(levelJS, "tileSet", tileSet);
	sj_object_insert(levelJS, "tileWidth", tileDimensions);
	sj_object_insert(levelJS, "tileHeight", tileDimensions);
	sj_object_insert(levelJS, "tileFPL", tileFPL);
	sj_object_insert(levelJS, "tileMap", level_array);
	//Player
	sj_object_insert(levelJS, "playerSpawn", player_start);
	//Enemies
	sj_object_insert(levelJS, "enemies", enemies);
	sj_object_insert(levelJS, "enemySpawns", enemy_spawns);
	//Pickups
	sj_object_insert(levelJS, "items", items);
	sj_object_insert(levelJS, "itemSpawns", item_spawns);
	//Interactables
	sj_object_insert(levelJS, "interactables", interactables);
	sj_object_insert(levelJS, "interactSpawns", interact_spawns);
	sj_object_insert(levelJS, "needKey", has_key);
	sj_object_insert(levelJS, "switchEffect", switch_effects);


	sj_object_insert(json, "level", levelJS);

	sj_save(json, "levels/customLevel.json");
	slog("level saved");
	return level_new();
}

void level_editor_update()
{
	gf2d_font_draw_line_tag(&count_text, FT_H1, gfc_color(0, 0, 0, 255), vector2d(600, 100));

	if (gfc_input_command_held("modifier"))
	{
		if (gfc_input_command_down("decrement") && inputTimer == 10)
		{
			count-= 10;
			count_real-= 10;
			sprintf(&count_text, "%d", count);
			inputTimer = 0;
		}
		if (gfc_input_command_down("increment") && inputTimer == 10)
		{
			count+= 10;
			count_real+= 10;
			sprintf(&count_text, "%d", count);
			inputTimer = 0;
		}
	}
	else if (gfc_input_command_down("decrement") && inputTimer == 10)
	{
		count--;
		count_real--;
		sprintf(&count_text, "%d", count);
		inputTimer = 0;
	}
	else if (gfc_input_command_down("increment") && inputTimer == 10)
	{
		count++;
		count_real++;
		sprintf(&count_text, "%d", count);
		inputTimer = 0;
	}

	if (inputTimer < 10)
		inputTimer++;
}

void level_editor_test(Level *level)
{

}

void new_yes_no_window(char * text)
{
	_yesNo = window_yes_no(text, onYes, onNo, NULL, NULL);
}

void level_editor_reset()
{
	_editor = window_level_editor("Level Editor:", onTile, onPlayer, onEnemy, onPickup, onInteract, onBackground, onTest, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	status = 0;
	pos = -1;
}