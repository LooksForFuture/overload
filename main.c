#include <gamedef.h>

#include <r_main.h>

#include <entity.h>
#include <component.h>
#include <transform.h>

#include <SDL2/SDL.h>

#include <stdio.h>

#define return_defer(val) do { result = (val); goto defer; } while (0)

void cleanup(void)
{
	r_shutdown();
}

int main(void)
{
	bool quit = false;
	atexit(cleanup);

	r_init();
	transform_init();

	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					r_update_window();
					break;
				}
				break;
			}
		}

		r_set_draw_color(0, 0, 0, 0);
		r_clear();

		r_set_draw_color(40, 40, 40, 255);
		r_set_viewport((viewport_t){0, 0, 1, 1, 1});
		r_fill_rect(&(SDL_FRect){0, 0, ASPECT_RATIO, 1});

		r_set_draw_color(255, 0, 0, 255);
		r_set_viewport((viewport_t){0, 0, 1, 1, 10});
		r_fill_rect(&(SDL_FRect){0, 0, 1, 1});

		r_present();
	}

	return 0;
}
