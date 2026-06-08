#include <r_main.h>
#include <gamedef.h>

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static float window_width = 1280.0f;
static float window_height = 720.0f;
static float start_x = 0.0f;
static float start_y = 0.0f;

static SDL_Rect render_area;
static viewport_t viewport;
static float render_unit;

void r_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "failed to initialize sdl\n");
		exit(1);
	}

	window = SDL_CreateWindow(
		"TDShooter",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		window_width, window_height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
		);
	if (!window) {
		fprintf(stderr, "failed to create window\n");
		exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1,
				      SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		fprintf(stderr, "failed to create renderer\n");
		exit(1);
	}
	SDL_RenderSetVSync(renderer, 1);

	r_set_viewport((viewport_t){0, 0, 1, 1, 100});
}

void r_shutdown(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

SDL_Window *r_get_window(void)
{
	return window;
}

void r_handle_event(SDL_Event *e)
{
	switch (e->type) {
	case SDL_WINDOWEVENT:
		switch(e->window.event)
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			r_update_window();
			break;
		break;
	}
}

void r_update_window(void)
{
	int width, height;
	float w, h;
	SDL_GetWindowSize(window, &width, &height);
	window_width = (float)width;
	window_height = (float)height;
	if (window_width/window_height >= ASPECT_RATIO) {
		start_y = 0.0f;
		h = window_height;
		w = h * ASPECT_RATIO;
		start_x = (window_width - w) / 2.0f;
	} else {
		start_x = 0.0f;
		w = window_width;
		h = w / ASPECT_RATIO;
		start_y = (window_height - h) / 2.0f;
	}

	render_area.x = start_x;
	render_area.y = start_y;
	render_area.w = w;
	render_area.h = h;
	SDL_RenderSetViewport(renderer, &render_area);
	render_unit = viewport.h * render_area.h / viewport.hlength;
}

void r_set_viewport(viewport_t view)
{
	SDL_Rect coords;

	viewport = view;
	render_unit = view.h * render_area.h / view.hlength;
	coords.x = view.x * render_area.w + start_x;
	coords.y = view.y * render_area.h + start_y;
	coords.w = view.w * render_area.w;
	coords.h = view.h * render_area.h;
	SDL_RenderSetViewport(renderer, &coords);
}

void r_set_draw_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void r_clear(void)
{
	SDL_RenderClear(renderer);
}

void r_fill_rect(SDL_FRect *coords)
{
	SDL_FRect dest;

	dest.x = coords->x * render_unit;
	dest.y = coords->y * render_unit;
	dest.w = coords->w * render_unit;
	dest.h = coords->h * render_unit;

	SDL_RenderFillRectF(renderer, &dest);
}

void r_present(void)
{
	SDL_RenderPresent(renderer);
}
