#ifndef __OVERLAY_H__
#define __OVERLAY_H__

#include "gf2d_sprite.h"

/**
* @brief intializes overlay sprites
*/
void init_overlay();

/**
* @brief draws overlay sprites and follows player
* @param alpha the colorshift/opacity value of the light
*/
void draw_light(Vector4D alpha);

/**
* @brief turns the player's light on and off
*/
void toggle_light();

#endif
