#include <gamedef.h>
#include <r_main.h>

#include <all_components.h>
#include <entity.h>
#include <component.h>
#include <transform.h>
#include <srender.h>

#include <SDL2/SDL.h>

#include <stdio.h>

#define return_defer(val) do { result = (val); goto defer; } while (0)

#define X(name) name##_init();
void init_all_components(void)
{
	ALL_COMPONENTS(X)
}
#undef X

void cleanup(void)
{
	r_shutdown();
}

int main(void)
{
	bool quit = false;
	Entity ent1, ent2;
	transform t1, t2;
	srender r1, r2;

	atexit(cleanup);

	r_init();
	init_entities();
	init_all_components();

	ent1 = create_entity();
	t1 = transform_add(ent1);
	r1 = srender_add(ent1);
	transform_set_position(t1, (Vec2){1, 0});
	transform_set_scale(t1, (Vec2){1, 1});
	srender_set_color(r1, (Vec3){255, 255, 0});

	ent2 = create_entity();
	t2 = transform_add(ent2);
	r2 = srender_add(ent2);
	transform_set_position(t2, (Vec2){0, 0});
	transform_set_scale(t2, (Vec2){1, 1});
	srender_set_color(r2, (Vec3){255, 0, 0});

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

		r_set_viewport((viewport_t){0, 0, 1, 1, 10});
		/*r_fill_rect(&(SDL_FRect){0, 0, 1, 1});*/
		srender_draw_all();

		r_present();
	}

	destroy_entity(ent1);
	destroy_entity(ent2);

	return 0;
}
