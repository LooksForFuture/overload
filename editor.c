#ifndef NO_EDITOR

/*
  The focused window is always the one at the end of the stack which
  means that if we have 4 windows, then the 4th one is the focused one.
  When the focus changes, we swap the new focused window with the last.
 */

#include <editor.h>
#include <r_main.h>

#include <SDL2/SDL.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define MAX_EDITOR_WINDOW_COUNT 4

#define TEXT_HEIGHT 2
#define GLOBAL_MARGIN 0.1
#define GLOBAL_PADDING 0.1

#define TITLE_BAR_COLOR 165, 165, 165
#define WIN_BG_COLOR 213, 213, 213

typedef struct {
	float x, y, w, h; //presented in percetage of game window size
	const char *title;
	bool is_focused;
	bool is_dragged;
	float drag_offset_x, drag_offset_y;
} Window;

struct {
	Window windows[MAX_EDITOR_WINDOW_COUNT];
	int window_capacity;
	int window_count;

	bool is_dragging;

	/* window width and height */
	int width;
	int height;

	//mouse coordinatations
	int mx, mx_prev;
	int my, my_prev;

	//mouse buttons
	bool m[3], m_prev[3];
} context;

static bool is_point_in_rect(SDL_Rect *rect, int x, int y)
{
	return x >= rect->x && x <= rect->x + rect->w &&
		y >= rect->y && y <= rect->y + rect->h;
}

void editor_init(void)
{
	context.window_capacity = MAX_EDITOR_WINDOW_COUNT;
	context.window_count = 0;
	context.is_dragging = false;
	context.mx = 0;
	context.my = 0;
	context.m[0] = false;
	context.m[1] = false;
	context.m[2] = false;
	context.m_prev[0] = false;
	context.m_prev[1] = false;
	context.m_prev[2] = false;
}

int editor_new_window(const char *title)
{
	int index = context.window_count++;
	Window *win = &context.windows[index];
	win->title = title;
	win->x = 0;
	win->y = 0;
	win->w = 40;
	win->h = 64;
	win->is_focused = true;
	win->is_dragged = false;

	return index;
}

void editor_handle_sdl_event(SDL_Event *e)
{
	switch (e->type) {
	case SDL_MOUSEMOTION: {
		context.mx_prev = context.mx;
		context.mx = e->motion.x;
		context.my_prev = context.my;
		context.my = e->motion.y;
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP: {
		switch (e->button.button) {
		case SDL_BUTTON_LEFT:
			context.m[0] = e->type == SDL_MOUSEBUTTONDOWN ?
				1: 0; break;
		case SDL_BUTTON_RIGHT:
			context.m[2] = e->type == SDL_MOUSEBUTTONDOWN ?
				1: 0; break;
		}
	}
		break;
	case SDL_WINDOWEVENT: {
		switch (e->window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			context.width = e->window.data1;
			context.height = e->window.data2;
			break;
		}
		break;
	}
	}
}

void editor_begin_frame(void)
{
	if (context.m[0] && !context.m_prev[0]) {
		int focused_window = -1;
		for (int i = 0; i < context.window_count; i++) {
			Window *win = &context.windows[i];
			SDL_Rect rect;
			rect.x = win->x * context.width;
			rect.y = win->y * context.height;
			rect.w = win->w * context.width;
			rect.h = win->h * context.height;

			if (is_point_in_rect(&rect,
					     context.mx,
					     context.my))
				focused_window = i;
		}
		if (focused_window > -1 &&
		    focused_window != context.window_count-1) {
			Window win = context.windows[
				context.window_count - 1];
			context.windows[context.window_count - 1] =
				context.windows[focused_window];
			context.windows[focused_window] = win;
		}
	}
}

void editor_render(void)
{
	for (int i = 0; i < context.window_count; i++) {
		Window *win = &context.windows[i];

		float width = win->w + 2*GLOBAL_MARGIN;

		r_set_draw_color(TITLE_BAR_COLOR, 255);
		float title_height = TEXT_HEIGHT + 2*GLOBAL_MARGIN;
		SDL_FRect title_bar = (SDL_FRect){win->x, win->y,
			width, title_height};
		r_fill_rect(&title_bar);

		r_set_draw_color(WIN_BG_COLOR, 255);
		SDL_FRect win_rect = (SDL_FRect){win->x,
			win->y + title_height,
			width, win->h + 2*GLOBAL_MARGIN};
		r_fill_rect(&win_rect);
	}
}

#else /* NO_EDITOR */

void editor_init(void) {}

#endif /* NO_EDITOR */
