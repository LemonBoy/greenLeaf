#ifndef INPUT_H_
#define INPUT_H_

#include "types.h"

extern int input_enabled;

#include <SDL/SDL.h>
#include <stdio.h>

INLINE int input_setup()
{
	input_enabled = 0;
	return 1;
}

INLINE void input_pre_update()
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

INLINE void input_post_update()
{
}	

INLINE int input_poweron()
{
	if(input_enabled) {
		input_enabled = 0;
		return 1;
	}else{
		input_enabled = 1;
		return 1;
	}
}

#endif /* INPUT_H_ */
