#include <kb_input.h>

#include <SDL2/SDL.h>

typedef struct {
	SDL_Scancode move_up;
	SDL_Scancode move_down;
	SDL_Scancode move_left;
	SDL_Scancode move_right;
	SDL_Scancode dodge;
	SDL_Scancode wp_next;
	SDL_Scancode wp_prev;
} KeyConfig;

static KeyConfig input_config;

static KeyConfig config_load(const char *path)
{
	KeyConfig cfg = {
		.move_up = SDL_SCANCODE_W,
		.move_down = SDL_SCANCODE_S,
		.move_left = SDL_SCANCODE_A,
		.move_right = SDL_SCANCODE_D,
		.dodge = SDL_SCANCODE_LCTRL,
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
				else if (!strcmp(key, "dodge"))
					cfg.dodge = sc;
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

void kb_input_update(const SDL_Event *e, InputProfile *p)
{
	switch (e->type) {
	case SDL_KEYDOWN:
		if (e->key.keysym.scancode == input_config.move_up)
			p->movement.y = 1;
		else if (e->key.keysym.scancode ==
			 input_config.move_down) p->movement.y = -1;
		else if (e->key.keysym.scancode ==
			 input_config.move_left) p->movement.x = -1;
		else if (e->key.keysym.scancode ==
			 input_config.move_right) p->movement.x = 1;
		else if (e->key.keysym.scancode ==
			 input_config.dodge) p->digital.dodge = true;
		break;

	case SDL_KEYUP:
		if (e->key.keysym.scancode == input_config.move_up)
			p->movement.y = 0;
		else if (e->key.keysym.scancode ==
			 input_config.move_down) p->movement.y = 0;
		else if (e->key.keysym.scancode ==
			 input_config.move_left) p->movement.x = 0;
		else if (e->key.keysym.scancode ==
			 input_config.move_right) p->movement.x = 0;
		else if (e->key.keysym.scancode ==
			 input_config.dodge) p->digital.dodge = false;
		break;
	}
}

void kb_input_init(void)
{
	input_config = config_load("input_config.txt");
}

void kb_input_shutdown(void)
{}
