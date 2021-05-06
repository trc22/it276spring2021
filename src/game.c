#include <SDL.h>
#include <stdio.h>

#include <simple_logger.h>
#include "gfc_input.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_windows.h"
#include "gf2d_entity.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "camera.h"
#include "gf2d_space.h"
#include "gf2d_collision.h"
#include "windows_common.h"
#include "gf2d_dynamic_body.h"

#include "level.h"
#include "player.h"
#include "pickup.h"
#include "interactables.h"

static int _done = 0;
static Window *_quit = NULL;
static Window *_mainMenu = NULL;

int selection = 0;

void onCancel(void *data)
{
	selection = 0;
	_mainMenu = NULL;
    _quit = NULL;
}
void onExit(void *data)
{
    _done = 1;
    _quit = NULL;
}

void onStart(void *data)
{
	gf2d_window_free(_mainMenu);
	selection = 1;
}

void onLoad(void *data)
{
	selection = 2;
}

void onQuit(void *data)
{
	selection = -1;
	_quit = NULL;

}

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int i;
    int fullscreen = 0;
    int debug = 0;
    Sprite *background = NULL;
	
    Space *space = NULL;
    Collision collision;
    CollisionFilter filter= {0};
	Entity *ent;
    int mx,my;
    float mf;
	Level *level;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i],"--fullscreen") == 0)
        {
            fullscreen = 1;
        }
        if (strcmp(argv[i],"--debug") == 0)
        {
            debug = 1;
        }
    }
    
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        fullscreen,
        debug);
    gf2d_graphics_set_frame_delay(16);
    gfc_audio_init(256,16,4,1,1,1);
    gf2d_sprite_init(1024);
    gf2d_action_list_init(128);
    gf2d_font_init("config/font.cfg");
    gfc_input_init("config/input.cfg");
    gf2d_windows_init(128);
    gf2d_entity_system_init(1024);

	init_pickup_system(10);
	interactable_system_init(10);
	load_all_items(50);

    
    camera_set_dimensions(vector2d(1200,700));
    
    SDL_ShowCursor(SDL_DISABLE);
    // game specific setup
        // init mouse, editor window
    gf2d_mouse_load("actors/mouse.actor");
   /*     space = gf2d_space_new_full(
        3,
        gf2d_rect(0,0,1200,700),
        0.1,
        vector2d(0,0.1),
        1,
        1);
    mf = 0;*/

	level = level_load("levels/menu.json", vector2d(600, 600), -1); //main menu level

    /*main game loop*/
  //  filter.worldclip = 1;
    /*main game loop*/
    while(!_done)
    {
        gfc_input_update();
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)
        {
            mf = 0;
        }
        gf2d_windows_update_all();
                
        gf2d_entity_think_all();
		gf2d_entity_update_all();
        gf2d_mouse_update();
       // gf2d_space_update(space);    
		level_update(level);
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
				// DRAW WORLD
				level_draw(level);
                // Draw entities
				gf2d_entity_draw_all();
            //UI elements last

            gf2d_font_draw_line_tag("Press F4 to quit!",FT_H1,gfc_color(255,255,255,255), vector2d(0,0));

            
            gf2d_windows_draw_all();
            gf2d_mouse_draw();
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
		if ((_quit == NULL && gfc_input_command_down("exit")) || (_quit == NULL && selection == -1))
        {
            _quit = window_yes_no("Exit?",onExit,onCancel,NULL,NULL);
        }
		
		if (selection == 0 && _mainMenu == NULL)
			_mainMenu = window_main_menu("Main Menu", onStart, onLoad, onQuit, NULL, NULL, NULL);

		if (selection == 1)
		{
			free(level);
			level = level_load("levels/exampleLevel.json", vector2d(300, 500), 0); //demo level
			selection = -2;
		}

  //    slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
