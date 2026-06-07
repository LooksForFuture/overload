#ifndef EDITOR_H
#define EDITOR_H

#ifndef NO_EDITOR

#include <SDL2/SDL.h>

void editor_init(void);

int editor_new_window(const char*);

void editor_handle_sdl_event(SDL_Event *);

void editor_begin_frame(void);

void editor_render(void);

#endif /* NO_EDITOR */
#endif /* EDITOR_H */
