#include "simple_logger.h"

#include "player.h"
#include "camera.h"
#include "level.h"

#include "overlay.h"

void player_update(Entity *self);
void player_think(Entity *self);

Vector2D player_position;
Entity *player;
Item *current_item;

int lightTimer, jumpTimer;
int inventoryPos, cycleTimer;

Entity *player_spawn(Vector2D position)
{
    Entity *ent;
    ent = entity_new();
    if (!ent)
    {
        slog("failed to create entity for the player");
        return NULL;
    }
	inventoryPos = 5;
	load_all_items(12);
	inventory_init(6);
	inventory_insert(get_item_by_id(0));
	check_empty(get_current_item(0));
	inventory_insert(get_item_by_id(1));
	inventory_insert(get_item_by_id(2));
	current_item = cycle_items();
	cycleTimer = 35;

	ent->sprite = gf2d_sprite_load_all("images/ed210_top.png",128,128,16);
    vector2d_copy(ent->position,position);
    ent->frameRate = 0.1;
    ent->frameCount = 16;
    ent->update = player_update;
    ent->think = player_think;
    ent->rotation.x = 64;
    ent->rotation.y = 64;
	ent->type = 0; //Type 0 = player
	//ent->last_collision.x = 0;
	//ent->last_collision.y = 0;
	ent->_canJump = false;
	lightTimer = 20;
	jumpTimer = 25;
	player = ent;
	ent->_touchingWall = false;
    return ent;
}


void player_update(Entity *self)
{
    Vector2D camera;
    Vector2D cameraSize;
    
    if (!self)return;
    cameraSize = camera_get_dimensions();
    camera.x = (self->position.x + 64) - (cameraSize.x * 0.5);
    camera.y = (self->position.y + 64) - (cameraSize.y * 0.5);
	camera_set_position(camera);
    // apply dampening on velocity
    vector2d_scale(self->velocity,self->velocity,0.75);
    if (vector2d_magnitude_squared(self->velocity) < 2)
    {
        vector2d_clear(self->velocity);
    }
	player_position = self->position;
}

void player_think(Entity *self)
{
    const Uint8 *keys;
    Vector2D aimdir,camera;
    int mx,my;
    if (!self)return;
    keys = SDL_GetKeyboardState(NULL);
    SDL_GetMouseState(&mx,&my);
    camera = camera_get_position();
    mx += camera.x;
    my += camera.y;
	
    
    // turn aimdir into a unit vector
    vector2d_normalize (&aimdir);

	if (!self->_touchingTile)
	{
		if (!self->_isJumping)
			self->velocity.y += 1.5;
		self->_canJump = false;
		//slog("Collision");
	}
	else
	{
		self->_touchingTile = false;
		self->_isJumping = false;
		self->_canJump = true;
	}

	if (keys[SDL_SCANCODE_A]) // move left
	{
		self->rotation.z = -90;

		if (self->last_collision.x >= self->position.x)
			self->_touchingWall = false;
		if (!self->_touchingWall)
			self->velocity.x -= 2;
	}

	if (keys[SDL_SCANCODE_D]) // move right
	{
		self->rotation.z = 90;

		if (self->last_collision.x <= self->position.x)
			self->_touchingWall = false;
		if (!self->_touchingWall)
			self->velocity.x += 2;
	}

	if (keys[SDL_SCANCODE_SPACE] && self->_canJump) //jump
	{
		jumpTimer = 0;
		self->_canJump = false;
		self->_isJumping = true;
	}
	if (jumpTimer <= 20)
	{
		self->_isJumping = true;
		if (self->velocity.x == 0)
			self->position.y -= 2;
		else
			self->velocity.y -= 1.5;
		jumpTimer++;
	}
	else
		self->_isJumping = false;

	if (keys[SDL_SCANCODE_TAB] && cycleTimer == 35)
	{
		current_item = cycle_items();
		cycleTimer = 0;
	}
	else if (cycleTimer != 35)
		cycleTimer++;

	if (keys[SDL_SCANCODE_E])
		use_item(current_item);

	//Overlay/light stuff
	if (lightTimer == 20)
	{
		if (keys[SDL_SCANCODE_F])
		{
			toggle_light(); //turns the light on/off
			lightTimer = 0;
		}
	}
	else
		lightTimer++;

}

void use_item(Item *item)
{
	if (item == NULL)
	{
		slog("Using null item");
		return;
	}
	if(item->itemID == NULL)
	{
		slog("null item id");
		return;
	}
	if(item->itemName == NULL)
	{
		slog("null item name");
		return;
	}
	slog("Using %s", item->itemName);
	if (item->quantity > 0)
		item->quantity--;
	slog("Quantity: %i", item->quantity);
	check_empty(item);
	if (!item->_inuse)
		return;
	//use item
			
}

Item *cycle_items()
{
	if (inventoryPos < 5)
		inventoryPos++;
	else
		inventoryPos = 0;

	slog("slot %i selected", inventoryPos);
	return get_current_item(inventoryPos);
}

Entity *get_player()
{
	return player;
}

Vector2D player_get_position()
{
	return player_position;
}


/**/
