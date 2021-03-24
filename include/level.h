#ifndef __LEVEL_H__
#define __LEVEL_H__

#include<SDL_rect.h>

#include "gfc_types.h"
#include "gfc_text.h"

#include "gf2d_sprite.h"
#include "entity.h"

typedef enum
{
    TT_Empty,
    TT_Brick,
    TT_MAX
}TileTypes;

typedef struct
{
    Sprite     **bgImage;    /**<the background image for the level*/
    int         bgImageCount;/**<how many we have*/
    Vector2D    levelSize;   /**<how large, in pixels, the level is*/
    Sprite     *tileSet;     /**<sprite for the tileset*/
    TileTypes  *tileMap;     /**<the tiles for the level*/
    Uint32      tileCount;
    Uint32      levelWidth;  /**<how many tiles per row the level has*/
    Uint32      levelHeight; /**<how many tiles per column the level has*/
    int         tileWidth;   /**<now many pixels wide the tiles are*/
    int         tileHeight;  /**<how many pixels tall each tile is*/
    int         tileFPL;
}Level;


/**
 * @brief allocate and initialize a level
 * @return NULL on error, else a pointer to a new level
 */
Level *level_new();


/**
 * @brief load a level from a json file
 * @param filename the file to load
 * @return NULL on error (See the logs) a loaded level otherwise
 */
Level *level_load(const char *filename, Vector2D playerSpawn, int levelID);

/**
 * @brief free up a previously loaded level
 * @param level a pointer to the level to free
 */
void level_free(Level *level);

/**
 * @brief perform maintenance for the level.  should be called once a frame
 * @param level the level to update
 */
void level_update(Level *level);


/**
 * @brief draw a level
 * @param level the level the draw
 */
void level_draw(Level *level);

Bool tile_collisions(SDL_Rect player, SDL_Rect collisionBox);

void level_transition(Level *level, Entity *door);

Level *get_current_level();

#endif
