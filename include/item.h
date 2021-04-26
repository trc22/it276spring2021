#ifndef __ITEM_H__
#define __ITEM_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "simple_json.h"


typedef struct Item_s
{
	Bool		_inuse;
	Bool		_consumable;
	Bool		_equipped;
	Bool		_hasAmmo;
	char		*itemName;
	int			itemID;
	Vector2D	itemSize;
	int			rotate90;
	int			quantity;
	int			max_quantity;
	int			timer;
	int			timerMax;
	int			ammoID;
	void		(*free)(struct Item_s *self);
	void         *data;
}Item;

/**
 * @brief allocates data for all items and stores them in an array for easy access
 * @param max_items the max amount of items allocated in memory
 */
void load_all_items(Uint32 max_items);

/**
* @brief frees the item list
*/
void items_free();

/**
* @brief allocates memory for the tetris style inventory
*/
void init_inventory_tetris();

/**
* @brief inserts item into tetris inventory
* @param location where in the inventory the item is inserted
*/
void item_insert_tetris(Vector2D location);

/**
* @brief frees the inventory
*/
void inventory_free();

#endif
