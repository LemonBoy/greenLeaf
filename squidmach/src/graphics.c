#include <OpenGL/gl.h>
#include <SDL/SDL.h>
#include <stdio.h>

#include "graphics.h"
#include "types.h"

static SDL_Surface *screen;
static int gfx_mode = 0;
static int gfx_enabled = 0;
static int o = 0;
static int delay = 0;

void graphics_clearscreen()
{
	SDL_FillRect(screen, NULL, 0);
}

int graphics_setup()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",
			SDL_GetError());
		return 0;
	}
	
	Uint32 flags = SDL_DOUBLEBUF | SDL_HWSURFACE;
	screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, 16, flags);
	if(screen == NULL) {
		fprintf(stderr, "Couldn't set %dx%dx16 video mode: %s\n", SCREEN_X, SCREEN_Y, 
			SDL_GetError());
		SDL_Quit();
		return 0;
	}
	
	graphics_clearscreen();

	gfx_mode = 0;
	gfx_enabled = 0;
	return 1;
}

static int graphics_mode_text()
{
	if(gfx_enabled == 0)
		return 0;
	if(gfx_mode == 1)
		return 0;
	
	/* TODO: Switch to text */
	
	gfx_mode = 1;
	return 1;
}

static int graphics_mode_video()
{
	if(gfx_enabled == 0)
		return 0;
	if(gfx_mode == 0)
		return 0;
	
	/* TODO: Switch to video */
	
	gfx_mode = 0;
	return 1;
}

int graphics_set_mode(int mode)
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

void graphics_pre_update()
{
	graphics_clearscreen();
}

void graphics_post_update()
{
	int t;
	if(o > 0) {
		t = SDL_GetTicks();
		delay += ((1000 / SCREEN_FPS) - (t - o));
		o = t;
		if(delay < 0)
			delay = 1000 / SCREEN_FPS;
	}else{
		o = SDL_GetTicks();
	}
	SDL_Delay(delay);
	if(gfx_enabled) {
		SDL_Flip(screen);
	}
}	

int graphics_poweron()
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

int graphics_printf(char* str, ...)
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

