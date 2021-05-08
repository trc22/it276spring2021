#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <SDL_rect.h>

#include "gf2d_entity.h"

#include "item.h"
#include "pickup.h"


/**
 * @brief spawn a player entity
 * @param position the screen position to spawn the player at
 * @return NULL on error, or a pointer to a new player entity
 */
Entity *player_spawn(Vector2D position);

void use_item(Item *item);

Item *cycle_items();

Entity *get_player();

Vector2D player_get_position();

int player_inventory_insert(Item *item);

Item *get_current_item();

int player_use_item(Item *item);

#endif
