#include <stdlib.h>
#include <SDL_rect.h>

#include "simple_logger.h"

#include "camera.h"
#include "entity.h"

typedef struct
{
    Entity *entity_list;
    Uint32  max_entities;
}EntityManager;

static EntityManager entity_manager = {0};

void entity_manager_init(Uint32 max_entities)
{
    if (max_entities == 0)
    {
        slog("cannot allocate 0 entities!");
        return;
    }
    if (entity_manager.entity_list != NULL)
    {
        entity_manager_free();
    }
    entity_manager.entity_list = (Entity *)gfc_allocate_array(sizeof (Entity),max_entities);
    if (entity_manager.entity_list == NULL)
    {
        slog("failed to allocate entity list!");
        return;
    }
    entity_manager.max_entities = max_entities;
    atexit(entity_manager_free);
    slog("entity system initialized");
}

void entity_manager_free()
{
    if (entity_manager.entity_list != NULL)
    {
        free(entity_manager.entity_list);
    }
    memset(&entity_manager,0,sizeof(EntityManager));
    slog("entity system closed");
}

void entity_update(Entity *self)
{
    if (!self)return;
    // DO ANY GENERIC UPDATE CODE
    vector2d_add(self->position,self->position,self->velocity);
    self->frame += self->frameRate;
    if (self->frame >= self->frameCount)self->frame = 0;
    // IF THERE IS A CUSTOM UPDATE, DO THAT NOW
    if (self->update)self->update(self);
}

void entity_manager_update_entities()
{
    int i;
    if (entity_manager.entity_list == NULL)
    {
        slog("entity system does not exist");
        return;
    }
    for (i = 0; i < entity_manager.max_entities; i++)
    {
        if (entity_manager.entity_list[i]._inuse == 0)continue;
        entity_update(&entity_manager.entity_list[i]);
    }
}

void entity_manager_think_entities()
{
    int i;
    if (entity_manager.entity_list == NULL)
    {
        slog("entity system does not exist");
        return;
    }
    for (i = 0; i < entity_manager.max_entities; i++)
    {
        if (entity_manager.entity_list[i]._inuse == 0)continue;
        if (entity_manager.entity_list[i].think != NULL)
        {
            entity_manager.entity_list[i].think(&entity_manager.entity_list[i]);
        }
    }
}

void entity_manager_draw_entities()
{
    int i;
	int j;
    if (entity_manager.entity_list == NULL)
    {
        slog("entity system does not exist");
        return;
    }

	for (i = 0; i < entity_manager.max_entities; i++)
    {
        if (entity_manager.entity_list[i]._inuse == 0)continue;
		entity_draw(&entity_manager.entity_list[i]);
		if (&entity_manager.entity_list[i] != NULL && &entity_manager.entity_list[(i + 1)] != NULL)
		{
			if (entity_clip(&entity_manager.entity_list[i], &entity_manager.entity_list[(i + 1)]))
				slog("Collision");
		}
    }

}


Entity *entity_new()
{
    int i;
    if (entity_manager.entity_list == NULL)
    {
        slog("entity system does not exist");
        return NULL;
    }
    for (i = 0;i < entity_manager.max_entities;i++)
    {
        if (entity_manager.entity_list[i]._inuse)continue;// someone else is using this one
        memset(&entity_manager.entity_list[i],0,sizeof(Entity));
        entity_manager.entity_list[i]._inuse = 1;
        return &entity_manager.entity_list[i];
    }
    return NULL;
}

void entity_free(Entity *ent)
{
    if (!ent)
    {
        slog("cannot free a NULL entity");
        return;
    }
    gf2d_sprite_free(ent->sprite);
    ent->sprite = NULL;
    ent->_inuse = 0;
}

void entity_draw(Entity *ent)
{
    Vector2D drawPosition,offset;
    if (!ent)
    {
        slog("cannot draww a NULL entity");
        return;
    }
    if (ent->draw)
    {
        ent->draw(ent);
    }
    else
    {
        if (ent->sprite == NULL)
        {
            return;// nothing to draw
        }
        offset = camera_get_offset();
        if (!camera_rect_on_screen(gfc_sdl_rect(ent->position.x,ent->position.y,ent->sprite->frame_w,ent->sprite->frame_h)))
        {
            //entity is off camera, skip
            return;
        }
		

        drawPosition.x = ent->position.x + offset.x;
        drawPosition.y = ent->position.y + offset.y;

        gf2d_sprite_draw(
            ent->sprite,
            drawPosition,
            NULL,
            NULL,
            &ent->rotation,
            NULL,
            NULL,
            (Uint32)ent->frame);
    }
	
	if (ent->type == 0) //If is player
	{
		ent->collisionBoxBody = gfc_sdl_rect(ent->position.x + 10, ent->position.y, ent->sprite->frame_w - 10, ent->sprite->frame_h / 2);
		ent->collisionBox = gfc_sdl_rect(ent->position.x + 50, ent->position.y, ent->sprite->frame_w - 100, ent->sprite->frame_h  - 5);
	}
	else
		ent->collisionBox = gfc_sdl_rect(ent->position.x, ent->position.y, ent->sprite->frame_w, ent->sprite->frame_h);
	
}

Bool entity_clip(Entity *a, Entity *b)
{
	if (a == NULL || b == NULL)
	{
		slog("Cannot collide with null entities");
		return false;
	}

	if (a == b)
	{
		slog("Cannot collide with self");
		return false;
	}

	int left_a, left_b;
	int right_a, right_b;
	int top_a, top_b;
	int bot_a, bot_b;

	left_a = a->collisionBox.x;
	right_a = (a->collisionBox.x + a->collisionBox.w);
	top_a = a->collisionBox.y;
	bot_a = (a->collisionBox.y + a->collisionBox.h);

	left_b = b->collisionBox.x;
	right_b = (b->collisionBox.x + b->collisionBox.w);
	top_b = b->collisionBox.y;
	bot_b = (b->collisionBox.y + b->collisionBox.h);

	if (bot_a <= top_b)
	{
		//a->collide(b);
		return false;
	}
	if (top_a >= bot_b)
	{
		//a->collide(b);
		return false;
	}
	if (right_a <= left_b)
	{
		
		//a->collide(b);
		return false;
	}

	if (left_a >= right_b)
	{
		//a->collide(b);
		return false;	
	}

	return true;
}



/*eol@eof*/
