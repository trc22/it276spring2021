#include "simple_logger.h"

#include "player.h"
#include "camera.h"
#include "overlay.h"
#include "font.h"
#include "item.h"

Bool _isOn;
Bool _showInventory;
Sprite* light;
Sprite* letterbox;
Sprite* darkness;
Vector2D light_pos;
Vector2D light_offset;
Vector4D lightColor = { 255, 255, 255, 245 };
Font *font_inventory;
Font *font_items;

int currentSlot;

Color hud_color, hud_selected;

void init_overlay()
{
	font_inventory = font_load("fonts/Warenhaus-Standard.ttf", 24);
	font_items = font_load("fonts/Warenhaus-Standard.ttf", 16);
	
	light = gf2d_sprite_load_all("images/LightOverlay.png", 1224, 1200, 1);
	darkness = gf2d_sprite_load_all("images/Black.png", 1200, 720, 1);
	light_pos = vector2d(0, 0);
	light_offset.x = 550;
	light_offset.y = 550;

	hud_color = gfc_color8(150, 150, 150, 255);
	hud_selected = gfc_color8(255, 0, 0, 255);
	
	currentSlot = 0;

	_isOn = true;
	_showInventory = false;
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

void draw_hud()
{
	inventory_text();
}

void update_hud_inventory(int inventory_pos)
{
	currentSlot = inventory_pos;
}

void inventory_text()
{
	if (_showInventory)
	{
		int i;
		char *slot_text;

		font_render(font_inventory, "Inventory:", vector2d(32, 32), gfc_color8(150, 150, 150, 255)); //draw inventory title

		for (i = 0; i < 6; i++)
		{
			if (get_current_item(i)->itemID == NULL)
				slot_text = "empty slot";//continue;
			else
				slot_text = get_current_item(i)->itemName;

			if (currentSlot == i)
				font_render(font_items, slot_text, vector2d(32, 64 + (i * 20)), hud_selected);
			else
				font_render(font_items, slot_text, vector2d(32, 64 + (i * 20)), hud_color);
		}
	}
}

void draw_overlay()
{
	draw_light(lightColor);
	draw_hud();
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

void toggle_inventory()
{
	if (_showInventory)
	{
		_showInventory = false;
		return;
	}
	_showInventory = true;
}