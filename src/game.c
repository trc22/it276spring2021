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
#include "gfc_audio.h"

#include "level.h"
#include "player.h"
#include "pickup.h"
#include "interactables.h"
#include "level_editor.h"
#include "save.h"
#include "overlay.h"

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
	gf2d_window_free(_mainMenu);
	selection = 2;
}

void onLevelEdit(void *data)
{
	gf2d_window_free(_mainMenu);
	selection = 3;
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
	Sound *sound;
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
	//gfc_audio_init(10, 2, 2, 1, true, false);
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

	level = level_load("levels/menu.json", -1); //main menu level
	sound = gfc_sound_load("sounds/short_jingle.mp3", 0.5, 1);
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


			if (selection == -3)
				level_editor_update();

			draw_overlay();
            gf2d_windows_draw_all();
            gf2d_mouse_draw();
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
		if ((_quit == NULL && gfc_input_command_down("exit")) || (_quit == NULL && selection == -1))
        {
            _quit = window_yes_no("Exit?",onExit,onCancel,NULL,NULL);
        }

		if (selection == -1)
		{
			_quit = window_yes_no("Exit?", onExit, onCancel, NULL, NULL);
		}

		
		if (selection == 0 && _mainMenu == NULL)
		{
			gfc_sound_play(sound, 1, .5, -1, -1);
			_mainMenu = window_main_menu("Main Menu", onStart, onLoad, onLevelEdit, NULL, NULL, NULL);
		}

		if (selection == 1)
		{
			level_free(level);
			level = level_load("levels/exampleLevel.json", 0); //demo level
			init_overlay();
			selection = -2;
		}

		if (selection == 2)
		{
			level = save_load_level(level);
			save_load_player();
			init_overlay();
			selection = -2;
		}

		if (selection == 3)
		{
			level_free(level);
			level = level_load("levels/customLevel.json", -1); //demo level
			level_editor_init(level);
			selection = -3;
		}

  //    slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
