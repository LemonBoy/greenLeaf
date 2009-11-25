#include <OpenGL/gl.h>
#include <SDL/SDL.h>
#include <stdio.h>

#include "input.h"
#include "types.h"

static int input_enabled;
static int o = 0;
static int delay = 0;

int input_setup()
{
	input_enabled = 0;
	return 1;
}

void input_pre_update()
{
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		switch(e.type) {
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			case SDL_KEYDOWN:
			case SDL_QUIT:
#ifdef DEBUG
				printf("Quitting...\n");
#endif
				exit(0);
				break;
			default:
				break;
		}
	}
}

void input_post_update()
{
}	

int input_poweron()
{
	if(input_enabled) {
		input_enabled = 0;
		return 1;
	}else{
		input_enabled = 1;
		return 1;
	}
}

