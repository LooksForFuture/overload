#ifndef EDITOR_H
#define EDITOR_H

#ifndef NO_EDITOR

#include <SDL2/SDL.h>

#include <stdbool.h>

#define ED_MAC_STRINGIFY(x) #x

#ifndef ED_FILE_LINE
#ifdef _MSC_VER
#define ED_FILE_LINE __FILE__ ":" ED_MAC_STRINGIFY(__COUNTER__)
#else
#define ED_FILE_LINE __FILE__ ":" ED_MAC_STRINGIFY(__LINE__)
#endif
#endif

enum ED_ALIGN {ED_ALIGN_LEFT, ED_ALIGN_CENTER, ED_ALIGN_RIGHT};

void editor_init(void);

void editor_shutdown(void);

int editor_strlen(const char *);

SDL_Window *editor_get_window(void);

void editor_begin_poll(void);

void editor_handle_event(SDL_Event *);

void editor_end_poll(void);

void editor_render(void);

bool editor_begin(const char *, SDL_Rect *);

void editor_end(void);

enum ed_tree_type {ED_TREE_NODE, ED_TREE_TAB};
enum ed_collapse_states {ED_MINIMIZED, ED_MAXIMIZED};

#define editor_tree_push(type, title, state) editor_tree_push_hashed(type, title, state, ED_FILE_LINE, editor_strlen(ED_FILE_LINE), __LINE__)

#define editor_tree_push_id(type, title, state, id) editor_tree_push_hashed(type, title, state, ED_FILE_LINE, nk_strlen(ED_FILE_LINE), id)

bool editor_tree_push_hashed(enum ed_tree_type, const char *,
			     enum ed_collapse_states, const char *,
			     int, int);

void editor_tree_pop(void);

void editor_layout_row_dynamic(int, int);

void editor_label(const char *, int);

bool editor_button(const char *);

int editor_propertyi(const char *, int, int, int);

float editor_propertyf(const char *, float, float, float);

bool editor_checkbox(const char *, bool);

bool editor_option_label(const char *, bool);

#endif /* NO_EDITOR */
#endif /* EDITOR_H */
