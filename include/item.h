#ifndef __ITEM_H__
#define __ITEM_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "simple_json.h"
#include "gf2d_sprite.h"



typedef struct Item_s
{
	Sprite		*sprite;
	Bool		_inuse;
	Bool		_usable;
	Bool		_equipped;
	Bool		_hasAmmo;
	char		*itemName;
	int			itemID;
	Vector2D	itemSize;
	Vector2D	pos;
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

void free_item(Item *item);

Item *item_load(int id, char *name, char *sprite, Bool usable, Bool hasAmmo, int ammoID, int max, int quantity, Vector2D size, int cooldown);

Item *get_item_by_id(int id);


void inventory_init(Uint32 max_items);

void inventory_insert_item(Item *item);

void inventory_remove_item(Item *item);

Item *get_item_by_pos(int pos);

Item *search_inventory(int id);

int get_item_inventory_pos(Item *item);


/**
* @brief allocates memory for the tetris style inventory
*/
void init_inventory_tetris();

/**
* @brief inserts item into tetris inventory
* @param item the item to insert
* @param location where in the inventory the item is inserted (row, column)
*/
void item_insert_tetris(Item *item, Vector2D location);

/**
* @brief removes an item from tetris inventory
* @param location where in the inventory the item is inserted
*/
void item_remove_tetris(Item *item);

/**
* @brief checks what item is at a given location
* @param location where in the inventory to check (row, column)
*/
Item *item_find_tetris(Vector2D location);


/**
* @brief moves an item into a new slot
* @param item the item to move
* @param location where the item will be moved to (row, column)
*/
void item_move_tetris(Item *item, Vector2D location);

/**
* @brief rotates an item 90 degrees
* @param item the item to rotate
*/
void item_rotate_tetris(Item *item);


/**
* @brief frees the inventory
*/
void inventory_free();


void draw_inventory();

void draw_cursor_inventory(Vector2D location);

#endif
