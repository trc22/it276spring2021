#include "simple_logger.h"

#include "player.h"
#include "camera.h"
#include "level.h"

#include "overlay.h"
#include "weapons.h"
#include "save.h"

void player_update(Entity *self);
void player_think(Entity *self);
void player_collide(Entity *self, Entity *other);

void player_load_save(const char *filename);


Vector2D player_position;
Entity *player;
Item *current_item;

int jumpTimer, cycleTimer, inventoryTimer, loadTimer;
int inventoryPos;

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
	inventory_init(6);
	inventory_insert(get_item_by_id(0));
	check_empty(get_current_item(0));
	inventory_insert(get_item_by_id(1));
	inventory_insert(get_item_by_id(14));
	inventory_insert(get_item_by_id(2));
	inventory_insert(get_item_by_id(9));
	inventory_insert(get_item_by_id(5));
	inventory_insert(get_item_by_id(7));
	current_item = cycle_items();

	ent->sprite = gf2d_sprite_load_all("images/ed210_top.png",128,128,16);
    vector2d_copy(ent->position,position);
    ent->frameRate = 0.1;
    ent->frameCount = 16;
    ent->update = player_update;
    ent->think = player_think;
	ent->collide = player_collide;
    ent->rotation.x = 64;
    ent->rotation.y = 64;
	ent->type = 0; //Type 0 = player
	//ent->last_collision.x = 0;
	//ent->last_collision.y = 0;
	ent->health = 100;
	ent->_canJump = false;
	player = ent;
	ent->_touchingWall = false;
	ent->_canCollide = true;
	ent->collisionTimer = 100; //using this for collisions

	//init timers
	jumpTimer = 25;
	cycleTimer = 35;
	inventoryTimer = 35;
	loadTimer = 200;

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

	//Use item
	if (keys[SDL_SCANCODE_F])
	{
		use_item(current_item);
	}

	if (keys[SDL_SCANCODE_I] && inventoryTimer == 35)
	{
		toggle_inventory();
		inventoryTimer = 0;
	}

	if (keys[SDL_SCANCODE_X])
	{
		drop_item(current_item, vector2d(player->position.x - 100, player->position.y + 50));
	}

	if (keys[SDL_SCANCODE_R])
	{
		slog("player pos: %f, %f", player_get_position().x, player_get_position().y);
	}


	if (keys[SDL_SCANCODE_L] && loadTimer == 200)
	{
		player_load_save("saves/save.json");
		loadTimer = 0;
	}
	
	update_timers();
	if(cycleTimer != 35)
		cycleTimer++;
	if (inventoryTimer != 35)
		inventoryTimer++;
	if (loadTimer != 200)
		loadTimer++;
	if (self->collisionTimer != 100)
		self->collisionTimer++;
	else
		self->_canCollide = true;
}

void player_collide(Entity *self, Entity *other)
{
	const Uint8 *keys;
	if (!self || !other)return;
	keys = SDL_GetKeyboardState(NULL);

	if (other->type == 2)
	{
		slog("touching enemy!");
		entity_damage(self, 10);
		player->velocity.y -= 50;
		if (player->velocity.x != 0)
			player->velocity.x += (player->velocity.x * -1);
	}

	if (other->type == 11)
	{
		inventory_insert(get_item_by_id(other->itemID));
		if (!check_inventory(other->itemID))
			return;

		entity_free(other);
	}

	if (keys[SDL_SCANCODE_E])
	{
		if (other->type == 21 || other->type == 22 ) //if button or door
		{
			other->_usable = true;
			other->collide(other, self);
		}

	}

	self->collisionTimer = 0;
	self->_canCollide = false;
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
	if (!item->_inuse)
		return;

	if (item->timer == item->timerMax && player->velocity.x == 0)
	{
		if (item->quantity > 0)
			item->quantity--;
		item->timer = 0;

		switch (item->itemID)
		{
		case 1:
			slog("Using pizza, only %i left", item->quantity);
			player->health += 20;
			slog("Player health is now: %i", player->health);
			break;
		case 2: //Use light
			toggle_light();
			break;
		case 4: //pistol
			fire_pistol(item, player->position, player->rotation.z);
			break;
		case 5: //shotgun
			fire_shotgun(item, player->position, player->rotation.z);
			break;
		case 6: //rifle
			fire_rifle(item, player->position, player->rotation.z);
		case 7: //dynamite
			fire_dynamite(item, player->position, player->rotation.z);
		case 12: //lighter
			break;
		case 13: //knife
			fire_knife(item, player->position, player->rotation.z);
			break;
		case 14: //Tape recorder
			check_empty(item); //Check empty before saving;
			new_save();
			break;
		default:
			break;
		}
	}
	check_empty(item);
}

Item *cycle_items()
{
	if (inventoryPos < 5)
		inventoryPos++;
	else
		inventoryPos = 0;

	slog("slot %i selected", inventoryPos);

	get_current_item(inventoryPos)->_equipped = true; //Equip current item
	
	if (inventoryPos < 0 ) 
		get_current_item(5)->_equipped = false;
	else
		get_current_item(inventoryPos - 1)->_equipped = false; //Unequip last item
	update_hud_inventory(inventoryPos);
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

int player_get_current_item()
{
	return inventoryPos;
}

void player_load_save(const char *filename)
{
	load_save(filename);
}


/**/
