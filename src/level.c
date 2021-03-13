#include <stdlib.h>

#include "simple_json.h"
#include "simple_logger.h"

#include "camera.h"
#include "level.h"

#include "player.h"

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

Level *level_load(const char *filename)
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
            level->tileFPL);
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
		collisionBox = gfc_sdl_rect(drawPosition.x, (drawPosition.y + 10), level->tileSet->frame_w, level->tileSet->frame_h);
		if (tile_collisions(get_player(), collisionBox))
		{
			get_player()->_touchingTile = true;
			get_player()->last_collision.x = collisionBox.x;
			get_player()->last_collision.y = collisionBox.y;
		}

	}
}

Bool tile_collisions(Entity *player, SDL_Rect collisionBox)
{
	if (player == NULL)
	{
		slog("Cannot collide with null entities");
		return false;
	}

	int left_a, left_b;
	int right_a, right_b;
	int top_a, top_b;
	int bot_a, bot_b;

	left_a = player->collisionBox.x;
	right_a = (player->collisionBox.x + player->collisionBox.h);
	top_a = player->collisionBox.y;
	bot_a = (player->collisionBox.y + player->collisionBox.h);

	left_b = collisionBox.x;
	right_b = (collisionBox.x + collisionBox.h);
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
/*file footer*/
