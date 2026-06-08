#ifndef EDITOR_H
#define EDITOR_H

#ifndef NO_EDITOR

#include <SDL2/SDL.h>

#include <stdbool.h>

void editor_init(void);

void editor_shutdown(void);

SDL_Window *editor_get_window(void);

void editor_begin_poll(void);

void editor_handle_event(SDL_Event *);

void editor_end_poll(void);

void editor_render(void);

bool editor_begin(const char *, SDL_Rect *);

void editor_end(void);

void editor_layout_row_dynamic(int, int);

bool editor_button(const char *);

#endif /* NO_EDITOR */
#endif /* EDITOR_H */
