#ifndef KB_INPUT_H
#define KB_INPUT_H

#include <input.h>

void kb_input_init(void);

void kb_input_update(const SDL_Event *, InputProfile *);

void kb_input_shutdown(void);

#endif /* KB_INPUT_H */
