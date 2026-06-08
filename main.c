#include <gamedef.h>
#include <glut.h>
#include <r_main.h>
#include <editor.h>

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

#define X(name) name##_shutdown();
void shutdown_all_components(void)
{
	ALL_COMPONENTS(X)
}
#undef X

#define X(name) name##_flush_entities();
void flush_all_components(void)
{
	ALL_COMPONENTS(X)
}
#undef X

void cleanup(void)
{
	#ifndef NO_EDITOR
	editor_shutdown();
	#endif

	shutdown_all_components();
	r_shutdown();
	shutdown_entities();
}

int main(void)
{
	bool quit = false;
	Entity ent1, ent2;
	transform t1, t2;
	srender r1, r2;

	atexit(cleanup);

	glut_init();
	r_init();
	init_entities();
	init_all_components();

	#ifndef NO_EDITOR
	editor_init();
	#endif

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
		SDL_Event e;
		editor_begin_poll();
		while (SDL_PollEvent(&e)) {
			SDL_Window *target_window = NULL;
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEWHEEL:
				target_window = SDL_GetWindowFromID(
					e.button.windowID
					);
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				target_window = SDL_GetWindowFromID(
					e.window.windowID
					);
				break;
			case SDL_WINDOWEVENT:
				target_window = SDL_GetWindowFromID(
					e.window.windowID
					);
				switch (e.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					quit = true;
					break;
				}
				break;
			case SDL_TEXTINPUT:
				target_window = SDL_GetKeyboardFocus();
				break;
			}

			if (target_window == r_get_window()) {
				r_handle_event(&e);
			}
#ifndef NO_EDITOR
			else if (target_window == editor_get_window()) {
				editor_handle_event(&e);
			}
#endif
		}
		editor_end_poll();

		flush_entities();
		flush_all_components();

		r_set_draw_color(0, 0, 0, 0);
		r_clear();

		r_set_draw_color(40, 40, 40, 255);
		r_set_viewport((viewport_t){0, 0, 1, 1, 1});
		r_fill_rect(&(SDL_FRect){0, 0, ASPECT_RATIO, 1});

		r_set_viewport((viewport_t){0, 0, 1, 1, 10});
		srender_draw_all();

		/* render the editor */
#ifndef NO_EDITOR
		if (editor_begin("Demo", &(SDL_Rect){30, 30, 150, 250})) {
			editor_layout_row_dynamic(30, 1);
			if (editor_button("push me")) {
				printf("I was pushed\n");
			}
		}
		editor_end();
		editor_render();
#endif /* NO_EDITOR */

		r_present();
	}

	return 0;
}
