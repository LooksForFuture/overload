#ifndef NO_EDITOR

#include <editor.h>

#include <SDL2/SDL.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include <nuklear.h>
#include <nuklear_sdl_renderer.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600

SDL_Window *win;
SDL_Renderer *renderer;

struct nk_context *ctx;
struct nk_colorf bg;

void editor_init(void)
{
	float font_scale = 1;

	win = SDL_CreateWindow("sdl_renderer",
			       SDL_WINDOWPOS_CENTERED,
			       SDL_WINDOWPOS_CENTERED,
			       WINDOW_WIDTH, WINDOW_HEIGHT,
			       SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
	if (!win) {
		fprintf(stderr, "failed to create editor window: %s\n",
			SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(win, -1,
		SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		fprintf(stderr, "failed to create editor renderer: %s\n",
			SDL_GetError());
		exit(1);
	}

	{
		int render_w, render_h;
		int window_w, window_h;
		float scale_x, scale_y;
		SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
		SDL_GetWindowSize(win, &window_w, &window_h);
		scale_x = (float)(render_w) / (float)(window_w);
		scale_y = (float)(render_h) / (float)(window_h);
		SDL_RenderSetScale(renderer, scale_x, scale_y);
		font_scale = scale_y;
	}

	ctx = nk_sdl_init(win, renderer);
	{
		struct nk_font_atlas *atlas;
		struct nk_font_config config = nk_font_config(0);
		struct nk_font *font;

		nk_sdl_font_stash_begin(&atlas);
		font = nk_font_atlas_add_default(atlas, 13 * font_scale,
						 &config);
		nk_sdl_font_stash_end();
		font->handle.height /= font_scale;
		nk_style_set_font(ctx, &font->handle);
	}

	bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
}

void editor_shutdown(void)
{
	nk_sdl_shutdown();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
}

SDL_Window *editor_get_window(void) {
	return win;
}

void editor_begin_poll(void)
{
	nk_input_begin(ctx);
}

void editor_handle_event(SDL_Event *e)
{
	nk_sdl_handle_event(e);
}

void editor_end_poll(void)
{
	nk_sdl_handle_grab();
	nk_input_end(ctx);
}

void editor_render(void)
{
	SDL_SetRenderDrawColor(renderer, bg.r * 255,bg.g * 255,
			       bg.b * 255, bg.a * 255);
	SDL_RenderClear(renderer);
	nk_sdl_render(NK_ANTI_ALIASING_ON);
	SDL_RenderPresent(renderer);
}

bool editor_begin(const char *title, SDL_Rect *rect)
{
	return nk_begin(ctx, title, nk_rect(rect->x, rect->y,
					    rect->w,rect->h),
		NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
			NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE);
}

void editor_end()
{
	nk_end(ctx);
}

void editor_layout_row_dynamic(int height, int count)
{
	nk_layout_row_dynamic(ctx, height, count);
}

bool editor_button(const char *title)
{
	return nk_button_label(ctx, title);
}

#else /* NO_EDITOR */

void editor_init(void) {}

void editor_shutdown(void) {}

#endif /* NO_EDITOR */
