#include <gamedef.h>
#include <glut.h>
#include <r_main.h>
#include <editor.h>
#include <gmath.h>

#include <physics.h>

#include <input.h>
#include <kb_input.h>

#include <all_components.h>
#include <entity.h>
#include <component_interfaces.h>
#include <transform.h>
#include <srender.h>
#include <collider.h>
#include <intent.h>
#include <actuator.h>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <string.h>

#define return_defer(val) do { result = (val); goto defer; } while (0)

static InputProfileStruct game_input;

void cleanup(void)
{

	#ifndef NO_EDITOR
	editor_shutdown();
	#endif

	shutdown_all_components();
	kb_input_shutdown();
	ph_shutdown();
	shutdown_entities();
	r_shutdown();
	glut_shutdown();
}

int main(void)
{
	bool quit = false;
	Entity player, camera, enemy1;

	InputMapper input_mappers[] = {
		(InputMapper){"kb & mouse", kb_input_update},
	};

	atexit(cleanup);

	glut_init();
	r_init();
	init_entities();
	ph_init();
	kb_input_init();
	init_all_components();

	#ifndef NO_EDITOR
	editor_init();
	#endif

	set_entity_component_interfaces(get_component_interfaces());

	/* setup the player */
	{
		transform t;
		srender s;
		collider c;
		intent i;
		actuator ac;
		player = create_entity();
		t = transform_add(player);
		s = srender_add(player);
		c = collider_add(player);
		i = intent_add(player);(void)i;
		ac = actuator_add(player);(void)ac;
		transform_set_position(t, (Vec2){0, 0});
		transform_set_scale(t, (Vec2){1, 1});
		srender_set_color(s, (Vec3){0, 255, 0});
		collider_set_velocity(c, (Vec2){0, 0});
	}

	/* setup the camera */
	{
		transform t;
		camera = create_entity();
		t = transform_add(camera);
		transform_set_position(t, (Vec2){0, 0});
	}

	/* setup enemy1 */
	{
		transform t;
		srender s;
		collider c;
		enemy1 = create_entity();
		t = transform_add(enemy1);
		transform_set_position(t, (Vec2){-3, 0});
		s = srender_add(enemy1);
		srender_set_color(s, (Vec3){255, 0, 0});
		c = collider_add(enemy1);
		collider_set_velocity(c, (Vec2){1, 0});
	}

	game_input = (InputProfileStruct){0};
	float current_time, previous_time, fixed_dt = 1.0f / 60.0f;
	float tick = 0.0f; //time passed since last fixed update
	current_time = SDL_GetTicks() / 1000.0f;
	previous_time = current_time;
	while (!quit) {
		current_time = SDL_GetTicks() / 1000.0f;
		float delta_time = current_time-previous_time;
		previous_time = current_time;
		if (delta_time > 0.1f) delta_time = 0.1f;

		/* event polling */
		SDL_Event e;
		editor_begin_poll();
		game_input.previous = game_input.current;
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
				input_mappers[0].update(&e,
					&game_input.current);
			}
#ifndef NO_EDITOR
			else if (target_window == editor_get_window()) {
				editor_handle_event(&e);
			}
#endif
		}
		editor_end_poll();

		/* fixed update */
		tick += delta_time;
		while (tick >= fixed_dt) {
			tick -= fixed_dt;

			collider_update_physics(fixed_dt);
		}

		/* update player input */
		{
			intent i = intent_get(player);
			intent_set_movement(
				i,
				(Vec2){game_input.current.movement.x,
				game_input.current.movement.y});
		}

		actuator_update();

		flush_entities();
		flush_all_components();

		r_set_draw_color(0, 0, 0, 0);
		r_clear();

		r_set_draw_color(40, 40, 40, 255);
		r_set_viewport((viewport_t){0, 0, 1, 1, 1});
		r_fill_rect(&(SDL_FRect){0, 0, ASPECT_RATIO, 1});

		r_set_viewport((viewport_t){0, 0, 1, 1,
				GAME_VIEW_HEIGHT});
		{
			transform t = transform_get(camera);
			Vec2 pos = transform_position(t);
			srender_draw_all(pos);
		}

#ifndef NO_EDITOR /* the editor */
		inspector_inspect(player);
		if (editor_begin("Bench", &(SDL_Rect){30, 280,
					250, 250})) {
			char buf[32];
			editor_layout_row_dynamic(0, 1);
			snprintf(buf, sizeof(buf), "frame time: %f", delta_time);
			editor_label(buf, ED_ALIGN_CENTER);
		} editor_end();
		editor_render();
#endif /* NO_EDITOR */

		r_present();
	}

	return 0;
}
