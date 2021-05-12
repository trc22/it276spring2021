#include "simple_logger.h"

#include "player.h"
#include "camera.h"
#include "overlay.h"
#include "item.h"

Bool _isOn;
Sprite* light;
Sprite* letterbox;
Sprite* darkness;
Vector2D light_pos;
Vector2D light_offset;
Vector4D lightColor = { 255, 255, 255, 245 };

int currentSlot;


void init_overlay()
{
//	font_inventory = font_load("fonts/Warenhaus-Standard.ttf", 24);
	//font_items = font_load("fonts/Warenhaus-Standard.ttf", 16);
	
	light = gf2d_sprite_load_all("images/LightOverlay.png", 1224, 1200, 1, false);
	darkness = gf2d_sprite_load_all("images/Black.png", 1200, 720, 1, false);
	light_pos = vector2d(0, 0);
	light_offset.x = 550;
	light_offset.y = 550;
	
	currentSlot = 0;

	_isOn = true;
	return;
}

void draw_light(Vector4D alpha)
{

	if (_isOn)
	{
		light_pos = vector2d((player_get_position().x - light_offset.x) - camera_get_position().x, (player_get_position().y - light_offset.y) - camera_get_position().y);

		gf2d_sprite_draw(light, light_pos, NULL, NULL, NULL, NULL, &alpha, 0);
		return;
	}
	
		gf2d_sprite_draw(darkness, vector2d(0,0), NULL, NULL, NULL, NULL, &alpha, 0);

}

void draw_overlay()
{
	draw_light(lightColor);
}

void toggle_light()
{
	if (_isOn)
	{
		_isOn = false;
		return;
	}
	_isOn = true;
}

Bool get_light_on()
{
	return _isOn;
}