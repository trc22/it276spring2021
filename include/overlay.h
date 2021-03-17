#ifndef __OVERLAY_H__
#define __OVERLAY_H__

#include "gf2d_sprite.h"

/**
* @brief intializes overlay sprites
*/
void init_overlay();

/**
* @brief turns the player's light on and off
*/
void toggle_light();

/**
* @brief Used to the light overlay and hud
*/
void draw_overlay();

void update_hud_inventory(int inventory_pos);

void inventory_text();


#endif
