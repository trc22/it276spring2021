#include <stdlib.h>

#include "simple_json.h"
#include "simple_logger.h"
#include "gf2d_collision.h"
#include "gf2d_space.h"

#include "camera.h"
#include "level.h"

#include "player.h"
#include "enemy.h"
#include "pickup.h"
#include "interactables.h"

//void level_spawns(int levelID);
//void level_starting_items(int levelID);

void level_add_shapes(Level *level);

void level_player_collisions(Level *level, Entity *player);

Level *currentLevel;
Collision collision;
CollisionFilter filter = { 1 };

Entity *ent_temp;

Level *level_new()
{
    Level *level;
    level = (Level *)malloc(sizeof(Level));
    if (!level)
    {
        slog("failed to allocate memory for the game level");
        return NULL;
    }
    memset(level,0,sizeof(Level));
    return level;
}

Level *level_load(const char *filename, int levelID)
{
    const char *string;
    Level *level;
    SJson *json,*levelJS,*levelMap,*row,*array;
	SJson *player_start, *enemies, *enemy_spawns, *enemy, *items, *item_spawns, *item;
	SJson *interactables, *interact, *interact_spawns, *has_key, *switch_effects, *effect;
    int rows,columns;
    int count,tileindex;
    int i,j;
    int tempInt;

	int x, y;
	int interact_x, interact_y;
	int enemy_type, item_id, interact_type, hasKey;

	Space *space = NULL;

	if (!filename)
    {
        slog("filename is NULL, cannot load the level");
        return NULL;
    }
	if (levelID == -1) return NULL;
    json = sj_load(filename);
    if (!json)return NULL;

    level = level_new();
    if (!level)
    {
        sj_free(json);
        return NULL;
    }
    
    levelJS = sj_object_get_value(json,"level");
    if (!levelJS)
    {
        slog("level json missing level object");
        level_free(level);
        sj_free(json);
        return NULL;
    }
    
    array = sj_object_get_value(levelJS,"bgImage");
    count = sj_array_get_count(array);
    level->bgImageCount = count;
    if (count)
    {
        level->bgImage = (Sprite **)gfc_allocate_array(sizeof(Sprite*),count);
        for (i = 0; i < count;i++)
        {
            string = sj_get_string_value(sj_array_get_nth(array,i));
            if (string)
            {
                level->bgImage[i] = gf2d_sprite_load_image((char *)string);
            }
        }
    }
    string = sj_get_string_value(sj_object_get_value(levelJS,"tileSet"));
    if (string)
    {
        slog("loading tile set %s",string);
        sj_get_integer_value(sj_object_get_value(levelJS,"tileWidth"),&level->tileWidth);
        sj_get_integer_value(sj_object_get_value(levelJS,"tileHeight"),&level->tileHeight);
        sj_get_integer_value(sj_object_get_value(levelJS,"tileFPL"),&level->tileFPL);
        level->tileSet = gf2d_sprite_load_all(
            (char *)string,
            level->tileWidth,
            level->tileHeight,
            level->tileFPL, false);
    }
    levelMap = sj_object_get_value(levelJS,"tileMap");
    if (!levelMap)
    {
        slog("missing tileMap data");
        level_free(level);
        sj_free(json);
        return NULL;
    }
    rows = sj_array_get_count(levelMap);
    row = sj_array_get_nth(levelMap,0);
    columns = sj_array_get_count(row);
    count = rows * columns;
    level->levelWidth = columns;
    level->levelHeight = rows;
    level->tileMap = (TileTypes*)gfc_allocate_array(sizeof(TileTypes),count);
    if (!level->tileMap)
    {
        level_free(level);
        sj_free(json);
        return NULL;
    }
    level->tileCount = count;
    
    tileindex = 0;
    slog("tilemap data:");
    for (j = 0; j < rows; j++)
    {
        row = sj_array_get_nth(levelMap,j);
        if (!row)continue;// skip it, its bad
        if (columns != sj_array_get_count(row))
        {
            slog("row %i, column count mismatch",j);
            continue;
        }
        for (i = 0; i < columns; i++)
        {
            sj_get_integer_value(sj_array_get_nth(row,i),&tempInt);
            level->tileMap[tileindex] = tempInt;
            printf("%i,",level->tileMap[tileindex++]);
        }
        printf("\n");
    }
    level->levelSize.x = level->levelWidth * level->tileWidth;
    level->levelSize.y = level->levelHeight * level->tileHeight;
    slog("map width: %f, with %i tiles wide, each %i pixels wide", level->levelSize.x, level->levelWidth,level->tileWidth);
    slog("map height: %f, with %i tiles high, each %i pixels tall", level->levelSize.y, level->levelHeight, level->tileHeight);
  
	player_start = sj_object_get_value(levelJS, "playerSpawn");
	sj_get_integer_value(sj_array_get_nth(player_start, 0), &x);
	sj_get_integer_value(sj_array_get_nth(player_start, 1), &y);

	slog("Player spawn: %i, %i", x, y);
	player_spawn(vector2d(x, y));


	enemies = sj_object_get_value(levelJS, "enemies");
	enemy_spawns = sj_object_get_value(levelJS, "enemySpawns");

	count = sj_array_get_count(enemies);
	slog("Spawning enemies");
	for (i = 0; i < count; i++)
	{
		sj_get_integer_value(sj_array_get_nth(enemies, i), &enemy_type);
		enemy = sj_array_get_nth(enemy_spawns, i);
			sj_get_integer_value(sj_array_get_nth(enemy, 0), &x);
			sj_get_integer_value(sj_array_get_nth(enemy, 1), &y);
			if (enemy_type == 0)
			{
				spawn_enemy_regular(vector2d(x, y));
				slog("Regular enemy spawned at: (%i, %i)", x, y);
			}
		}

		items = sj_object_get_value(levelJS, "items");
		item_spawns = sj_object_get_value(levelJS, "itemSpawns");

		count = sj_array_get_count(items);
		slog("spawning pickups:");
		for (i = 0; i < count; i++)
		{
			x = 0; y = 0;
			sj_get_integer_value(sj_array_get_nth(items, i), &item_id);
			item = sj_array_get_nth(item_spawns, i);
			sj_get_integer_value(sj_array_get_nth(item, 0), &x);
			sj_get_integer_value(sj_array_get_nth(item, 1), &y);
			if (item_id == 1)
			{
				slog("Item pickup at: (%i, %i)", x, y);
			//	pickup_spawn(vector2d(x, y), "flashlight", i, "pickup", 1);
			}
		}

		interactables = sj_object_get_value(levelJS, "interactables");
		interact_spawns = sj_object_get_value(levelJS, "interactSpawns");
		has_key = sj_object_get_value(levelJS, "hasKey");
		switch_effects = sj_object_get_value(levelJS, "switchEffect");

		count = sj_array_get_count(interactables);
		slog("spawning interactables:");
		for (i = 0; i < count; i++)
		{

			sj_get_integer_value(sj_array_get_nth(interactables, i), &interact_type);

			interact = sj_array_get_nth(interact_spawns, i);
			sj_get_integer_value(sj_array_get_nth(interact, 0), &x);
			sj_get_integer_value(sj_array_get_nth(interact, 1), &y);

			effect = sj_array_get_nth(switch_effects, i);
			sj_get_integer_value(sj_array_get_nth(effect, 0), &interact_x);
			sj_get_integer_value(sj_array_get_nth(effect, 1), &interact_y);

			sj_get_integer_value(sj_array_get_nth(has_key, i), &hasKey);
			
			if (interact_type == 0)
			{
				interactable_spawn(vector2d(x, y), IT_DOOR, "levels/exampleLevel.json", hasKey, vector2d(interact_x, interact_y));
				slog("Door spawned at: (%i, %i)", x, y);
			}
			else if (interact_type == 1)
			{
				interactable_spawn(vector2d(x, y), IT_SWITCH, "levels/exampleLevel.json", hasKey, vector2d(interact_x, interact_y));
				slog("Switch spawned at: (%i, %i)", x, y);
			}
			else if (interact_type == 2)
			{
				interactable_spawn(vector2d(x, y), IT_SAVE, "levels/exampleLevel.json", hasKey, vector2d(interact_x, interact_y));
				slog("Save point spawned at: (%i, %i)", x, y);
			}

		}
	

	sj_free(json);
	currentLevel = level;

	level_make_space();
	level_add_shapes(level);
	filter.worldclip = 1;
	filter.touchlayer = 1;

    return level;
}


void level_update(Level *level)
{
    SDL_Rect camera;
    if (!level)return;
    camera = camera_get_rect();
    //snap camera to the level bounds
    if ((camera.x + camera.w) > (int)level->levelSize.x)
    {
        camera.x = level->levelSize.x - camera.w;
    }
    if ((camera.y + camera.h) > (int)level->levelSize.y)
    {
        camera.y = level->levelSize.y - camera.h;
    }
    if (camera.x < 0)camera.x = 0;
    if (camera.y < 0)camera.y = 0;
    camera_set_position(vector2d(camera.x,camera.y));

	gf2d_entity_pre_sync_all();
	gf2d_space_update(currentLevel->space);
	gf2d_entity_post_sync_all();
}

void level_free(Level *level)
{
    int i;
    if (!level)return;// nothing to do
    
    if (level->tileMap != NULL)
    {
        free(level->tileSet);
        level->tileMap = NULL;
    }
    if (level->bgImageCount)
    {
        for (i = 0; i < level->bgImageCount;i++)
        {
            gf2d_sprite_free(level->bgImage[i]);
        }
        free(level->bgImage);
    }
    gf2d_sprite_free(level->tileSet);
    
    free(level);
}

void level_draw(Level *level)
{
    SDL_Rect camera;
	SDL_Rect collisionBox;
    Vector2D offset,drawPosition,parallax;
    int i;
    if (!level)
    {
        slog("cannot draw level, NULL pointer provided");
        return;
    }
    // draw the background first
    offset = camera_get_offset();
    if (level->bgImageCount)
    {
        camera = camera_get_rect();
        for (i = 0; i < level->bgImageCount;i++)
        {
            parallax.x = (float)(level->bgImage[i]->frame_w - camera.w)/ (level->levelSize.x - camera.w);
            parallax.y = (float)(level->bgImage[i]->frame_h - camera.h)/ (level->levelSize.y - camera.h);
            
            gf2d_sprite_draw_image(level->bgImage[i],vector2d(offset.x * parallax.x,offset.y * parallax.y));        
        }
            
    }
    //then draw the tiles
    
    if (!level->tileMap)
    {
        slog("not tiles loaded for the level, cannot draw it");
        return;
    }
    for (i = 0; i < level->tileCount; i++)
    {
        if (level->tileMap[i] == 0)continue;
        drawPosition.x = ((i % level->levelWidth)*level->tileSet->frame_w);
        drawPosition.y = ((i / level->levelWidth)*level->tileSet->frame_h);
        if (!camera_rect_on_screen(gfc_sdl_rect(drawPosition.x,drawPosition.y,level->tileSet->frame_w,level->tileSet->frame_h)))
        {
            //tile is off camera, skip
            continue;
        }
        drawPosition.x += offset.x;
        drawPosition.y += offset.y;
        gf2d_sprite_draw(
            level->tileSet,
            drawPosition,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            level->tileMap[i] - 1);
		drawPosition.x -= offset.x;
		drawPosition.y -= offset.y;
	}
}

void level_make_space()
{
	currentLevel->space = gf2d_space_new_full(3,
		gf2d_rect(0, 0, currentLevel->levelWidth, currentLevel->levelHeight),
		0.1, vector2d(0,0),
		1,
		0.001);
}

void level_add_shapes(Level *level)
{
	int i;
	Vector2D position;

	for (i = 0; i < level->tileCount; i++)
	{
		if (level->tileMap[i] == 0)continue;
		position.x = ((i % level->levelWidth)*level->tileSet->frame_w);
		position.y = ((i / level->levelWidth)*level->tileSet->frame_h);
		gf2d_space_add_static_shape(currentLevel->space, (gf2d_shape_rect(position.x, position.y, level->tileWidth, level->tileHeight)));
	}
}

void level_collisions(Level *level, Entity *ent)
{
	if (ent == NULL) return;
	if (ent->body.worldclip == 1) return;
	if (ent->id == 0)
	{
		level_player_collisions(level, ent);
		return;
	}
	if (ent->id != 6)
	{
		collision = gf2d_collision_trace_space(level->space, vector2d(ent->position.x, ent->position.y), vector2d(ent->position.x, ent->position.y + 54), filter);
		if (collision.collided && collision.pointOfContact.y > ent->position.y)
		{
			ent->grounded = 1;
		}
		else
		{
			ent->grounded = 0;
		}
	}
	//collision = gf2d_collision_trace_space(level->space, vector2d(ent->position.x - 6, ent->position.y + ent->actor.sprite->frame_h), vector2d(ent->actor.sprite->frame_w + 6, ent->position.y + ent->actor.sprite->frame_h), filter);
	collision = gf2d_collision_trace_space(level->space, vector2d(ent->position.x - 2, ent->position.y + ent->actor.sprite->frame_h / 2), vector2d(ent->position.x + 38, ent->position.y + ent->actor.sprite->frame_h / 2), filter);
	if (collision.collided)
	{
		if (collision.pointOfContact.x < ent->position.x)
			ent->canmove = 1;
		if (collision.pointOfContact.x > ent->position.x + ent->actor.sprite->frame_w)
			ent->canmove = -1;
	}
	else
		ent->canmove = 0;
}

void level_player_collisions(Level *level, Entity *player)
{

	collision = gf2d_collision_trace_space(level->space, vector2d(player->position.x + 16, player->position.y), vector2d(player->position.x + 16, player->position.y + 54), filter);
	if (collision.collided && collision.pointOfContact.y > player->position.y)
	{
		player->grounded = 1;
	}
	else
	{
		player->grounded = 0;
	}
	collision = gf2d_collision_trace_space(level->space, vector2d(player->position.x - 2, player->position.y + 26), vector2d(player->position.x + 38, player->position.y + 26), filter);
	if (collision.collided)
	{
		if (collision.pointOfContact.x < player->position.x)
			player->canmove = 1;
		if (collision.pointOfContact.x > player->position.x + 16)
			player->canmove = -1;
	}
	else
		player->canmove = 0;
}
void level_transition(char* level_name, int level_id)
{
	entity_free_all();
	slog("freed all ents");
	free(get_current_level());
	slog("freed level");
	level_load(level_name, level_id);
	slog("loaded new level");
}

Level *get_current_level()
{
	return currentLevel;
}

/*file footer*/
