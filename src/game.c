#include <SDL.h>

#include <math.h>

#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "font.h"
#include "camera.h"
#include "entity.h"
#include "player.h"
#include "level.h"
#include "overlay.h"
#include "enemy.h"
#include "pickup.h"
#include "save.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Level *level;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};

    
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
        0);
    gf2d_graphics_set_frame_delay(16);
    camera_set_dimensions(vector2d(1200,720));
    camera_set_position(vector2d(0,0));
    gf2d_sprite_init(1024);
	font_init(10);
    entity_manager_init(100);
	load_all_items(20);
    
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);
	level = level_load("levels/exampleLevel.json", vector2d(600, 600));
	init_overlay();
	spawn_enemy_regular(vector2d(2274, 1720));
	spawn_pickup(vector2d(2200, 1720), 7);
	spawn_pickup(vector2d(300, 1720), 3);
	spawn_door(vector2d(1800, 1720), "levels/exampleLevel.json");
	//load_save("saves/save.json");

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
        entity_manager_think_entities();
        entity_manager_update_entities();

        level_update(level);

        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            level_draw(level);
            
            entity_manager_draw_entities();


			draw_overlay();


            //UI elements last
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);
			
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
//        slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
