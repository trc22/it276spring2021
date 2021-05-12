# Tim's readme (IT 276)

Controls:
 - WASD: movement
 - SPACE: jump
 - E: interact
 - F: use item equipped item
 - X: drop item
 - I: show inventory
 - Arrow keys: navigate inventory
 - Enter: use/equip item
 - R: rotate item in inventory
 - TAB: cycle inventory modes (use/equip and move mode)
 - +/-: Increment/decrment keys for level editor
 - SHIFT: Modifier for level editor (allows you to inc/dec by 10 instead of 1)
 - L: load save

Current features
 - Hacky lighting
 - Puzzle elements (door/key and lever)
 - Five weapons (pistol, shotgun, knife, dynamite, rifle)
 - Sound effects
 - Unique items (flashlight, pizza, key, ammo, tape-recorder)
 - Spooky enemies (normal, small, big, tall, ranged)
 - Fully interactable environment 
 - Resident evil 4 style tetris inventory
 - Neat particle effects
 - Improved physics
 - The worst level editor ever made
 - Full read/write (can save inventory to a json file and load it)


Sources:
-All sprites/sfx/music are either locally sourced or from open game art
-https://opengameart.org/content/various-inventory-24-pixel-icon-set
-https://www.pinterest.com/pin/483503709968688595/



------------------------------------------------------------------------------------------------------------
# gameframework2d (GF2D)
a collection of utlitity functions designed to facilitate creating 2D games with SDL2
This project is specifically intended to function as an educational tool for my students taking 2D Game Programming.

Currently the project is in a WIP state, however it is functional with sample projects showcasing rudimentary scrolling shooters,
platformers, and adventure games.

The master branch is kept deliberately minimal to allow students to build out systems as they are learning.
Other branches have more complete system: Collisions, menus, input abstraction.

# Build Process

Before you can build the example code we are providing for you, you will need to obtain the libraries required
by the source code
 - SDL2
 - SDL2_image
 - SDL2_mixer
 - SDL2_ttf
There are additional sub modules that are needed for this project to work as well, but they can be pulled right from within the project.
Performable from the following steps from the root of the cloned git repository within a terminal. 

Make sure you fetch submodules: `git submodule update --init --recursive`
Go into each submodule's src directory and type:
`make`
`make static`

Once each submodule has been made you can go into the base project src folder anre simply type:
`make`

You should now have a `gf2d` binary within the root of your git repository. Executing this will start your game.
