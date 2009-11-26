#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "types.h"

int  graphics_setup();
int  graphics_poweron();
void graphics_pre_update();
void graphics_post_update();
int  graphics_set_mode(int mode);
int  graphics_printf(char* str, ...);

#endif /* GRAPHICS_H_ */
