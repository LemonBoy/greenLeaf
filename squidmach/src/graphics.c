#include "graphics.h"
#include "types.h"

SDL_Surface *gfx_screen;
int gfx_mode = 0;
int gfx_enabled = 0;
int gfx_o = 0;
int gfx_delay = 0;
u32 gfx_start_time;
u32 gfx_new_time;

