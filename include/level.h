#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_types.h"
#include "gfc_text.h"

#include "gf2d_sprite.h"

typedef enum
{
	TT_Empty,
	TT_Brick,
	TT_
}TileTypes;

typedef struct
{
	Sprite     *bgImage;     /**<the background image for the level*/
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
Level *level_load(const char *filename);

/**
* @brief free up a previously loaded level
* @param level a pointer to the level to free
*/
void level_free(Level *level);


/**
* @brief draw a level
* @param level the level the draw
*/
void level_draw(Level *level);

#endif
