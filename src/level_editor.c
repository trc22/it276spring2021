#include "simple_logger.h"
#include "simple_json.h"
#include "windows_common.h"
#include "gf2d_element_entry.h"
#include "gfc_input.h"
#include "gf2d_font.h"


#include "level_editor.h"

Level *current_level;

SJson *json, *levelJS, *levelMap, *row, *level_array, *tile;
SJson *player_start, *enemies, *enemy_spawns, *enemy, *items, *item_spawns, *item;
SJson *interactables, *interact, *interact_spawns, *has_key, *switch_effects, *effect;

int status, pos, count, count_real;

int level_length, level_width;

static Window *_textEntry = NULL;
static Window *_editor = NULL;
static Window *_yesNo = NULL;

List textList;

char count_text[100];

void new_yes_no_window(char * text);

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
				sj_object_insert(levelJS, "tileMap", level_array);
				level_editor_make();
			}
			return;
		default:
			return;
	}
}

void onNo(void *data)
{

}

void onTile(void *data)
{
	slog("tile");
	_editor = NULL;
	status = LE_TILES;
	pos = -1;
	count = 1;
	//_textEntry = window_text_entry("Level length:", "0", 5000, onYes, onNo);
	_yesNo = window_yes_no("Level length", onYes, onNo, NULL, NULL);
}

void onPlayer(void *data)
{
	slog("player");
	_editor = NULL;
}

void onEnemy(void *data)
{
	slog("enemy");
	_editor = NULL;

}
void onPickup(void *data)
{
	slog("pickup");
	_editor = NULL;
}

void onInteract(void *data)
{
	slog("interact");
	_editor = NULL;
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

	_editor = window_level_editor("Level Editor:", onTile, onPlayer, onEnemy, onPickup, onInteract, NULL, NULL, NULL, NULL, NULL);
	return level;
}

Level * level_editor_make()
{
	slog("making level");
	sj_object_insert(json, "level", levelJS);
	slog("inserted tile map");
	sj_save(json, "levels/customLevel.json");
	slog("level saved");
	return level_new();
}

void level_editor_update()
{
	//count_text = count;
	gf2d_font_draw_line_tag(&count_text, FT_H1, gfc_color(0, 0, 0, 255), vector2d(600, 100));
	
	if (gfc_input_command_down("decrement"))
	{
		count--;
		count_real--;
		sprintf(&count_text, "%d", count);
	}
	if (gfc_input_command_down("increment"))
	{
		count++;
		count_real++;
		sprintf(&count_text, "%d", count);
	}
}

void level_editor_test(Level *level)
{

}

void new_yes_no_window(char * text)
{
	_yesNo = window_yes_no(text, onYes, onNo, NULL, NULL);
}
