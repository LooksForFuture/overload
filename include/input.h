#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

#include <stdbool.h>

typedef struct {
	struct {
		float x;
		float y;
	} movement;

	struct {
		bool dodge;
		bool wp_next;
		bool wp_prev;
	} digital;
} InputProfile;

typedef struct {
	const char *name;
	void (*update)(const SDL_Event *, InputProfile *);
} InputMapper;

typedef struct {
	InputProfile current;
	InputProfile previous;
} InputProfileStruct;

#endif /* INPUT_H */
