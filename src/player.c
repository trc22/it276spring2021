#include "simple_logger.h"
#include "gfc_input.h"

#include "player.h"
#include "camera.h"
#include "level.h"
#include "gf2d_actor.h"
#include "gf2d_dynamic_body.h"

#include "item.h"
#include "weapon.h"
//#include "overlay.h"
//#include "weapons.h"
#include "save.h"

void player_update(Entity *self);
void player_think(Entity *self);
void player_draw(Entity *self);
int player_touch(Entity *self, Entity *other);
int player_damage(Entity *self, int amount, Entity *source);
void player_die(Entity *self);
void player_load_save(const char *filename);

void handle_inventory();
void handle_items(Item *item);
void handle_weapons(Item *item);

Vector2D player_position;
Entity *player;

int jumpTimer, cycleTimer, inventoryTimer, loadTimer;
int inventoryPos;

Vector2D slot, old_pos;
int inventoryMode, movingItem, old_z;
Item *current_item;
Item *equipped_item;

Bool i_open;

Entity *player_spawn(Vector2D position)
{
    Entity *ent;
	Shape shape;
	ent = gf2d_entity_new();
    if (!ent)
    {
        slog("failed to create entity for the player");
        return NULL;
    }
	slog("player spawned");
	ent->id = 0;
	ent->dead = 0;
	ent->inuse = 1;
	ent->grounded = 0;
	ent->canmove = 0;
	ent->cooldown = 0; //Invincibility cooldown
	/*inventoryPos = 5;
	inventory_init(6);
	inventory_insert(get_item_by_id(0));
	check_empty(get_current_item(0));
	current_item = cycle_items();*/

	gf2d_actor_free(&ent->actor);
	gf2d_actor_load(&ent->actor, "actors/player.actor");
	gf2d_actor_set_action(&ent->actor, "idle");
	
	shape = gf2d_shape_rect(ent->position.x, ent->position.y, ent->actor.size.x, ent->actor.size.y);
	ent->shape = shape;
	gf2d_body_clear(&ent->body);
	gf2d_body_set(
		&ent->body, 
		"player", 
		0, 
		WORLD_LAYER, 
		1, 
		0, 
		ent->position, 
		vector2d(0, 0), 
		10, 
		1, 
		1, 
		&ent->shape, 
		ent, 
		NULL);

	ent->body.team = 0;
	vector2d_copy(ent->position, position);
	ent->acceleration = vector2d(0, 0);
	ent->body.gravity = 0.5;

	shape = gf2d_shape_rect(ent->position.x, ent->position.y, 16, 16);
	/*ent->pe = gf2d_particle_emitter_new_full(
		100,
		1,
		1,
		PT_Sprite,
		ent->position,
		vector2d(2, 0),
		vector2d(0, -3),
		vector2d(2, 1),
		vector2d(0, 0.05),
		vector2d(0, 0.01),
		gfc_color(1, 0, 0, 1),
		gfc_color(1, 0, 0, 1),
		gfc_color(1, 0, 0, 1),
		&shape,
		0,
		0,
		0,
		"images/particle.png",
		16,
		16,
		1,
		0,
		SDL_BLENDMODE_ADD);*/

	ent->pe = gf2d_particle_emitter_new_full(
		1000,
		0,
		0,
		PT_Pixel,
		//vector2d(575, 340),
		vector2d(575, 340),
		vector2d(3, 0),
		vector2d(1, 0),
		vector2d(0.1, 0),
		vector2d(0, 0),
		vector2d(0, 0.01),
		gfc_color(1, 0, 0, 1),
		gfc_color(0, 0, 0, 0),
		gfc_color(1, 0, 0, 1),
		&shape,
		0,
		0,
		0,
		"images/particle.png",
		16,
		16,
		1,
		0,
		SDL_BLENDMODE_BLEND);

    ent->update = player_update;
	ent->think = player_think;
	ent->draw = player_draw;
	ent->touch = player_touch;
	ent->damage = player_damage;
	ent->die = player_die;
	
	inventory_init(16);
	init_inventory_tetris();
	//item_insert_tetris(get_item_by_id(1), vector2d(4, 3), 0);
	item_insert_tetris(get_item_by_id(4), vector2d(1, 1), 0);
	item_insert_tetris(get_item_by_id(6), vector2d(5, 5), 0);

	//inventory_remove_item(search_inventory(4));

	i_open = false;
	inventoryMode = 0;
	movingItem = 0;
	slot = vector2d(0, 0);
	old_pos = vector2d(0, 0);
	old_z = 0;

	current_item = get_item_by_id(0);
	equipped_item = get_item_by_id(0);
	
    ent->rotation.x = 64;
    ent->rotation.y = 64;
	ent->flip = vector2d(0, 0);
	ent->velocity = vector2d(0, 0);
	//ent->last_collision.x = 0;
	//ent->last_collision.y = 0;
	ent->health = 100;
	player = ent;

	//init timers
	jumpTimer = 25;
	cycleTimer = 35;
	inventoryTimer = 10;
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

	gf2d_particle_emitter_update(self->pe);
	player_position = self->position;
}

void player_think(Entity *self)
{
	const Uint8 *keys;
	Vector2D aimdir, camera;
	int mx, my;	
	if (!self)return;
	keys = SDL_GetKeyboardState(NULL);
	SDL_GetMouseState(&mx, &my);
	camera = camera_get_position();
	mx += camera.x;
	my += camera.y;

	if (gfc_input_command_pressed("reload"))
	{
		save_new("levels/exampleLevel.json", self->position);
	}

	if (gfc_input_command_released("walkright") || gfc_input_command_released("walkleft"))
	{
		if (!gfc_input_command_held("walkright") && !gfc_input_command_held("walkleft"))
		{
			self->velocity.x = 0;
			self->acceleration.x = 0;
			gf2d_actor_set_action(&self->actor, "idle");
		}
	}

	if (self->grounded == 0 && jumpTimer == 25)
	{
		self->acceleration.y = 0.05;
	}
	else
	{
		self->acceleration.y = 0;
		self->velocity.y = 0;
	}

	if (gfc_input_command_pressed("walkleft"))
	{
		gf2d_actor_set_action(&self->actor, "run");
	}
	if (gfc_input_command_held("walkleft") && self->canmove != 1)  // move left
	{

		self->acceleration.x = -0.05;
		self->flip = vector2d(-1, 0);
	}

	if (self->velocity.x < 0 && self->canmove == 1)
	{
		self->velocity.x = 0;
		self->acceleration.x = 0;
	}

	if (gfc_input_command_pressed("walkright"))
	{
		gf2d_actor_set_action(&self->actor, "run");
	}
	if (gfc_input_command_held("walkright") && self->canmove != -1) // move right
	{
		//self->velocity.x = 2;
		self->acceleration.x = 0.05;
		self->flip = vector2d(0, 0);
	}

	if (self->velocity.x > 0 && self->canmove == -1)
	{
		self->velocity.x = 0;
		self->acceleration.x = 0;
	}


	if (self->velocity.x > 3)
		self->velocity.x = 3;
	if (self->velocity.x < -3)
		self->velocity.x = -3;

	if (gfc_input_command_pressed("jump") && self->grounded) //jump
	{
		jumpTimer = 0;
	}
	if (jumpTimer < 25)
	{
		jumpTimer++;
		self->velocity.y -= 1;
	}

	if (gfc_input_command_pressed("inventory"))
	{
		if (!i_open)
			i_open = true;
		else
			i_open = false;
	}

	if (keys[SDL_SCANCODE_TAB] && cycleTimer == 35)
	{
		//current_item = cycle_items();
		cycleTimer = 0;
	}

	if (self->cooldown < 50)
	{
		self->body.team = 1;
		self->cooldown++;
	}
	else
	{
		self->body.team = 0;
	}

	if (gfc_input_command_pressed("use_item"))
		handle_weapons(equipped_item);

	handle_inventory();


	//Use item
	/*if (keys[SDL_SCANCODE_F])
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
	if (cycleTimer != 35)
		cycleTimer++;
	if (inventoryTimer != 35)
		inventoryTimer++;
	if (loadTimer != 200)
		loadTimer++;*/
}

int player_touch(Entity *self, Entity *other)
{
//	slog("Touching entity");
	return 1;
}

int player_damage(Entity *self, int amount, Entity *source)
{
	self->health -= amount;
	vector2d_add(self->velocity, self->velocity, source->velocity);
	self->cooldown = 0;
	if (self->health <= 0)
		self->die(self);
}

void player_die(Entity *self)
{
	Shape shape;
	Sprite *sprite;
	sprite = gf2d_sprite_load_image("images/particle.png");
	shape = gf2d_shape_rect(self->position.x, self->position.y, 16, 16);
	gf2d_entity_free(self);
}



void player_draw(Entity *self)
{
	//gf2d_body_draw(&self->body, vector2d(self->position.x - camera_get_position().x, self->position.y - camera_get_position().y));
	//gf2d_body_draw(db->body, vector2d(self->position.x - camera_get_position().x, self->position.y));
	if (i_open)
	{
		draw_inventory();
		draw_cursor_inventory(slot);
	}
}
/*void player_collide(Entity *self, Entity *other)
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
}*/


/*void use_item(Item *item)
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
			break;
		case 7: //dynamite
			fire_dynamite(item, player->position, player->rotation.z);
			break;
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
}*/


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
	//load_save(filename);
}

void handle_inventory()
{
	const Uint8 *keys;
	if (!i_open)
		return;

	keys = SDL_GetKeyboardState(NULL);

	if (gfc_input_command_pressed("inventory_mode"))
	{
		if (inventoryMode)
			inventoryMode = 0;
		else
			inventoryMode = 1;
	}

	if (gfc_input_command_pressed("ok"))
	{
		if (current_item->itemID == 0)
		{
			current_item = item_find_tetris(slot);
			slog("selected %s", current_item->itemName);
		}

		if (current_item != NULL && current_item->itemID > 0)
		{
			if (inventoryMode == 0)
			{
				if (current_item->_equippable)
				{
					slog("Equipping %s", current_item->itemName);
					equipped_item = search_inventory(current_item->itemID);
				}
				else if (current_item->_usable)
				{
					slog("Using %s", current_item->itemName);

				}
				current_item = get_item_by_id(0);
				return;
			}
			if (!movingItem && inventoryMode == 1)
			{
				slog("Moving %s", current_item->itemName);
				vector2d_copy(old_pos, current_item->pos);
				old_z = current_item->rotation->z;
				movingItem = 1;
				return;
			}
			if (movingItem && inventoryMode == 1)
			{
				if (!item_move_tetris(current_item, current_item->pos, slot))
				{
					current_item->rotation->z = old_z;
				}
				movingItem = 0;
				current_item = get_item_by_id(0);
				return;
			}

		}
	}

	if (gfc_input_command_pressed("reload") && movingItem && current_item->itemID > 0)
	{
		item_rotate_tetris(current_item);
	}

	if (gfc_input_command_pressed("drop_item"))
	{
		current_item = item_find_tetris(slot);
		if (current_item->itemID > 0)
		{
			slog("Removing %s", current_item->itemName);
			inventory_remove_item(current_item);
		}
	}

	if (inventoryTimer == 10)
	{
		if (keys[SDL_SCANCODE_UP])
			slot.x--;
		if (keys[SDL_SCANCODE_DOWN])
		{
			slot.x++;
		}
		if (keys[SDL_SCANCODE_LEFT])
			slot.y--;
		if (keys[SDL_SCANCODE_RIGHT])
			slot.y++;

		if (slot.x > 5)
			slot.x = 5;
		if (slot.x < 0)
			slot.x = 0;
		if (slot.y > 7)
			slot.y = 7;
		if (slot.y < 0)
			slot.y = 0;

		if (movingItem)
			search_inventory(current_item->itemID)->pos = slot;

		inventoryTimer = 0;
	}
	else
		inventoryTimer++;
}

int player_inventory_insert(Item *item)
{
	if (!i_open)
		i_open = 1;

	item->pos = vector2d(0, 0);

	inventoryMode = 1;
	movingItem = 1;

	current_item = item;
}

void handle_weapons(Item *item)
{
	if (item == NULL)
	{
		slog("Using null item");
		return;
	}
	if (item->itemID == NULL)
	{
		slog("null item id");
		return;
	}
	if (item->itemName == NULL)
	{
		slog("null item name");
		return;
	}
	if (!item->_inuse)
		return;

	if (player->velocity.x == 0)
	{
		if (item->quantity > 0)
			item->quantity--;
		item->timer = 0;

		switch (item->itemID)
		{
		case 2: //Use light
			//toggle_light();
			break;
		case 4: //pistol
			weapon_fire_pistol();
			break;
	/*	case 6: //rifle
			fire_rifle(item, player->position, player->rotation.z);
			break;*/
		default:
			break;
		}
	}
}


/**/
