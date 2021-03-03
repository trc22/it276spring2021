#include "simple_logger.h"

#include "player.h"
#include "camera.h"
#include "overlay.h"

Bool _isOn;
Sprite* light;
Sprite* letterbox;
Sprite* darkness;
Vector2D light_pos;
Vector2D light_offset;

void init_overlay()
{
	light = gf2d_sprite_load_all("images/LightOverlay.png", 1200, 1200, 1);
	darkness = gf2d_sprite_load_all("images/Black.png", 1200, 720, 1);
	light_pos = vector2d(0, 0);
	light_offset.x = 550;
	light_offset.y = -250;

	_isOn = true;
	return;
}

void draw_light()
{
	if (_isOn)
	{
		light_pos = vector2d((player_get_position().x - light_offset.x) - camera_get_position().x, light_offset.y);

		gf2d_sprite_draw_image(light, light_pos);

		return;
	}
	
	gf2d_sprite_draw_image(darkness, vector2d(0, 0));
	


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