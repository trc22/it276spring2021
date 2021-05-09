#include "simple_logger.h"
#include "simple_json.h"
#include "windows_common.h"

#include "level_editor.h"


Level *current_level;

SJson *json, *levelJS, *levelMap, *row, *array;
SJson *player_start, *enemies, *enemy_spawns, *enemy, *items, *item_spawns, *item;
SJson *interactables, *interact, *interact_spawns, *has_key, *switch_effects, *effect;

int status;

static Window *_textEntry = NULL;
static Window *_editor = NULL;
static Window *_yesNo = NULL;


void onYes(void *data)
{

}

void onNo(void *data)
{

}

void onTile(void *data)
{
	slog("tile");
}

void onPlayer(void *data)
{
	slog("player");
}

void onEnemy(void *data)
{
	slog("enemy");
}
void onPickup(void *data)
{
	slog("pickup");
}

void onInteract(void *data)
{
	slog("interact");
}

Level * level_editor_init()
{
	Level *level;
	level = level_new();
	status = LE_TILES;
	_editor = window_level_editor("Level Editor:", onTile, onPlayer, onEnemy, onPickup, onInteract, NULL, NULL, NULL, NULL, NULL);
}

Level * level_editor_make()
{

}

void level_editor_update()
{
}

void level_editor_test(Level *level)
{

}