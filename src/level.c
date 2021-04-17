#include <stdlib.h>

#include "simple_json.h"
#include "simple_logger.h"

#include "camera.h"
#include "level.h"

//#include "player.h"
//#include "enemy.h"
//#include "pickup.h"
//#include "interactables.h"

//void level_spawns(int levelID);
//void level_starting_items(int levelID);

Level *currentLevel;

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

Level *level_load(const char *filename, Vector2D playerSpawn, int levelID)
{
    const char *string;
    Level *level;
    SJson *json,*levelJS,*levelMap,*row,*array;
    int rows,columns;
    int count,tileindex;
    int i,j;
    int tempInt;

    if (!filename)
    {
        slog("filename is NULL, cannot load the level");
        return NULL;
    }
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
    sj_free(json);
	currentLevel = level;
//	level_spawns(levelID); //spawn all non-player entities
	if(levelID != -1)
		player_spawn(playerSpawn); //spawn player
//	level_starting_items(levelID); //setup player inventory
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
		/*collisionBox = gfc_sdl_rect(drawPosition.x, drawPosition.y, level->tileSet->frame_w, level->tileSet->frame_h);

		if (tile_collisions(get_player()->collisionBox, collisionBox))
		{
			get_player()->_touchingTile = true;
		}

		if (tile_collisions(get_player()->collisionBoxBody, collisionBox))
		{
			get_player()->_touchingWall = true;
			get_player()->last_collision.x = collisionBox.x;
			get_player()->last_collision.y = collisionBox.y;
		}

		enemy_physics(collisionBox);*/

	}
}
/*
Bool tile_collisions(SDL_Rect player, SDL_Rect collisionBox)
{
	int left_a, left_b;
	int right_a, right_b;
	int top_a, top_b;
	int bot_a, bot_b;

	left_a = player.x;
	right_a = (player.x + player.w);
	top_a = player.y;
	bot_a = (player.y + player.h);

	left_b = collisionBox.x;
	right_b = (collisionBox.x + collisionBox.w);
	top_b = collisionBox.y;
	bot_b = (collisionBox.y + collisionBox.h);

	if (bot_a <= top_b)
	{
		return false;
	}
	if (top_a >= bot_b)
	{
		return false;
	}
	if (right_a <= left_b)
	{
		return false;
	}

	if (left_a >= right_b)
	{
		return false;
	}
	return true;
}

void level_transition(Level *level, Entity *door)
{
	free_all_entities();
	slog("freed all ents");
	free(level);
	slog("freed level");
	level_load(door->destination, vector2d(600, 600), door->duration);
//	slog("loaded new level");
}

void level_spawns(int levelID)
{
	switch (levelID)
	{
	case 0: //Demo level
		
		//Enemies
		spawn_enemy_regular(vector2d(2274, 1720));
		spawn_enemy_small(vector2d(2350, 1720));
		spawn_enemy_big(vector2d(2400, 1650));
		spawn_enemy_tall(vector2d(2450, 1700));
		spawn_enemy_ranged(vector2d(2500, 1730));

		//Pickups
		spawn_pickup(vector2d(2200, 1720), 7);
		spawn_pickup(vector2d(300, 1720), 3);
		spawn_button(vector2d(600, 1700), "level_00_door");
		break;
	case 1:
		break;
	default:
		break;
	}

}

void level_starting_items(int levelID)
{
	//Setup player inventory
	if (levelID == 0)
	{
		inventory_insert(get_item_by_id(2));
		inventory_insert(get_item_by_id(4));
		inventory_insert(get_item_by_id(5));
		inventory_insert(get_item_by_id(8));
		inventory_insert(get_item_by_id(9));
		inventory_insert(get_item_by_id(13));
	}
	return;
}*/

Level *get_current_level()
{
	return currentLevel;
}

/*file footer*/
