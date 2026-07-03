#include <gamedef.h>
#include <glut.h>
#include <r_main.h>
#include <editor.h>
#include <gmath.h>

#include <all_components.h>
#include <entity.h>
#include <component_interfaces.h>
#include <transform.h>
#include <srender.h>
#include <collider.h>
#include <intent.h>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <string.h>

#define return_defer(val) do { result = (val); goto defer; } while (0)

typedef struct {
	SDL_Scancode move_up;
	SDL_Scancode move_down;
	SDL_Scancode move_left;
	SDL_Scancode move_right;
	SDL_Scancode sprint;
	SDL_Scancode wp_next;
	SDL_Scancode wp_prev;
} KeyConfig;

static uint8_t previous_keys[SDL_NUM_SCANCODES];
static uint8_t current_keys[SDL_NUM_SCANCODES];

static inline bool key_pressed(SDL_Scancode key)
{
	return current_keys[key];
}

static inline bool key_down(SDL_Scancode key)
{
	return current_keys[key] && !previous_keys[key];
}

static inline bool key_up(SDL_Scancode key)
{
	return !current_keys[key] && previous_keys[key];
}

KeyConfig config_load(const char *path)
{
	KeyConfig cfg = {
		.move_up = SDL_SCANCODE_W,
		.move_down = SDL_SCANCODE_S,
		.move_left = SDL_SCANCODE_A,
		.move_right = SDL_SCANCODE_D,
		.sprint = SDL_SCANCODE_LCTRL,
		.wp_next = SDL_SCANCODE_E,
		.wp_prev = SDL_SCANCODE_Q
	};

	FILE *f = fopen(path, "r");
	if (!f) return cfg;

	char line[256];
	while (fgets(line, sizeof(line), f)) {
		char key[64], value[64];
		if (sscanf(line, "%63[^=] = %63s", key, value) == 2) {
			SDL_Scancode sc = SDL_GetScancodeFromName(value);
			if (sc != SDL_SCANCODE_UNKNOWN) {
				if (!strcmp(key, "move_up"))
					cfg.move_up = sc;
				else if (!strcmp(key, "move_down"))
					cfg.move_down = sc;
				else if (!strcmp(key, "move_left"))
					cfg.move_left = sc;
				else if (!strcmp(key, "move_right"))
					cfg.move_right = sc;
				else if (!strcmp(key, "sprint"))
					cfg.sprint = sc;
				else if (!strcmp(key, "weapon_next"))
					cfg.wp_next = sc;
				else if (!strcmp(key, "weapon_prev"))
					cfg.wp_prev = sc;
			}
		}
	}
	fclose(f);

	return cfg;
}

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
	Entity player, camera;
	KeyConfig input_config;

	atexit(cleanup);

	glut_init();
	r_init();
	init_entities();
	init_all_components();

	#ifndef NO_EDITOR
	editor_init();
	#endif

	input_config = config_load("input_config.txt");

	memset(previous_keys, 0, sizeof(previous_keys));
	memset(current_keys, 0, sizeof(current_keys));

	/* setup the player */
	{
		transform t;
		srender s;
		collider c;
		intent i;
		player = create_entity();
		t = transform_add(player);
		s = srender_add(player);
		c = collider_add(player);
		i = intent_add(player);(void)i;
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

		/* update keyboard state */
		memcpy(previous_keys, current_keys,
		       sizeof(previous_keys));
		memcpy(current_keys, SDL_GetKeyboardState(NULL),
		       sizeof(current_keys));

		/* update player input */
		{
			intent i = intent_get(player);
			Vec2 movement = {0};
			if (key_pressed(input_config.move_up))
				movement.y = 1;
			if (key_pressed(input_config.move_down))
				movement.y = -1;
			if (key_pressed(input_config.move_left))
				movement.x = -1;
			if (key_pressed(input_config.move_right))
				movement.x = 1;

			intent_set_movement(i, movement);
		}

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
		editor_render();
#endif /* NO_EDITOR */

		r_present();
	}

	return 0;
}
