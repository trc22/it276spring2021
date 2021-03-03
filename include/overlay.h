#ifndef __OVERLAY_H__
#define __OVERLAY_H__

#include "gf2d_sprite.h"

/**
* @brief intializes overlay sprites
*/
void init_overlay();

/**
* @brief draws overlay sprites and follows player
*/
void draw_light();

/**
* @brief turns the player's light on and off
*/
void toggle_light();

#endif
