#ifndef R_MAIN_H
#define R_MAIN_H

/*
  The rendering subsystem.
  Can only render to a rectangular area which is cut out from the center
  of the window, based on the aspect ratio defined in gamedef.h while
  leaving the rest of the window blank.

  The (0, 0) coordinates are the top left. The X-axis points to right,
  while the Y-axis points to down.

  In order to render, one needs to define a viewport. More info about
  viewports can be found at the definition of viewport_t.

  Render unit: The viewport height can be divided into a number. The width
  then is calculated by multiplying the aspect ratio of the viewport with
  the specified vertical length. Let's take a look at an example.
  The vertical length is 5, and the aspect ratio of the viewport is 1.6
  So, the horizontal length would be 1.6 * 5 = 8. This means that the
  height of the viewport can contain 5, 1x1 squares, while the width can
  contain 8 squares.
  The render unit is calculated everytime a viewport is set.
 */

#include <SDL2/SDL.h>
#include <stdbool.h>

/*
  viewport
  The viewport x, y, w, and h attributes are defined based on percentages
  of the rendering area. For example the (0.5, 1) means at 50% of the
  width of the rendering area and 100% of the width. The default viewport
  is {0, 0, 1, 1, 100} which means that the viewport starts at (0, 0)
  and contains the whole rendering area while dividing the height to 100.
 */

typedef struct {
	float x, y, w, h, hlength;
} viewport_t;

void r_init(void);

void r_shutdown(void);

SDL_Window *r_get_window(void);

void r_handle_event(SDL_Event *e);

/* must be called when the window changes size */
void r_update_window(void);

/* set viewport */
void r_set_viewport(viewport_t);

/* set the rendering color */
void r_set_draw_color(Uint8, Uint8, Uint8, Uint8);

/* fills the whole window with the color */
void r_clear(void);

void r_fill_rect(SDL_FRect *coords);

/* presents the image to the window */
void r_present(void);

/* load a texture(image) from a file */
int r_load_texture(void);

#endif /* R_MAIN_H */
