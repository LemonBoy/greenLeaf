#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <SDL/SDL.h>
#include <stdio.h>
#include "types.h"

extern SDL_Surface *gfx_screen;
extern int gfx_mode;
extern int gfx_enabled;
extern int gfx_o;
extern int gfx_delay;
extern u32 gfx_start_time;
extern u32 gfx_new_time;

INLINE void graphics_clearscreen()
{
	SDL_FillRect(gfx_screen, NULL, 0);
}

INLINE int graphics_setup()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",
			SDL_GetError());
		return 0;
	}
	
	Uint32 flags = SDL_DOUBLEBUF | SDL_HWSURFACE;
	gfx_screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, 16, flags);
	if(gfx_screen == NULL) {
		fprintf(stderr, "Couldn't set %dx%dx16 video mode: %s\n", SCREEN_X, SCREEN_Y, 
			SDL_GetError());
		SDL_Quit();
		return 0;
	}
	
	graphics_clearscreen();
	
	gfx_start_time = SDL_GetTicks(); 
	gfx_mode = 0;
	gfx_enabled = 0;
	return 1;
}

INLINE int graphics_mode_text()
{
	if(gfx_enabled == 0)
		return 0;
	if(gfx_mode == 1)
		return 0;
	
	/* TODO: Switch to text */
	
	gfx_mode = 1;
	return 1;
}

INLINE int graphics_mode_video()
{
	if(gfx_enabled == 0)
		return 0;
	if(gfx_mode == 0)
		return 0;
	
	/* TODO: Switch to video */
	
	gfx_mode = 0;
	return 1;
}

INLINE int graphics_set_mode(int mode)
{
	switch(mode) {
		case 0:
			return graphics_mode_video();
		case 1:
			return graphics_mode_text();
		default:
			return 0;
	}
}

INLINE void graphics_pre_update()
{
	graphics_clearscreen();
}

INLINE void graphics_post_update()
{
	int gfx_t;
	if(gfx_o > 0) {
		gfx_t = SDL_GetTicks();
		gfx_delay += ((1000 / SCREEN_FPS) - (gfx_t - gfx_o));
		gfx_o = gfx_t;
		if(gfx_delay < 0)
			gfx_delay = 1000 / SCREEN_FPS;
	}else{
		gfx_o = SDL_GetTicks();
	}
	SDL_Delay(gfx_delay);
	if(gfx_enabled) {
		SDL_Flip(gfx_screen);
	}
	gfx_new_time = SDL_GetTicks();
	printf("%f fps\n", 1 / (float)(gfx_new_time - gfx_start_time) * 1000.0f);
	gfx_start_time = gfx_new_time;
}

INLINE int graphics_poweron()
{
	if(gfx_enabled) {
		graphics_clearscreen();
		gfx_enabled = 0;
		return 1;
	}else{
		graphics_clearscreen();
		gfx_enabled = 1;
		return graphics_set_mode(1);
	}
}

static int graphics_printf(char* str, ...)
{
	char text[256];
	va_list	ap;
	if(gfx_enabled != 1)
		return 0;
	if(gfx_mode != 1)
		return 0;
	if(text == NULL)
		return 0;
	va_start(ap, str);
        vsprintf(text, str, ap);
	va_end(ap);
	
	/* TODO: Actual printing here */
	printf("%s", text);
	fflush(stdout);
	return 1;
}

#endif /* GRAPHICS_H_ */
