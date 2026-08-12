#ifndef SRENDER_H
#define SRENDER_H

#include <component.h>
#include <gmath.h>

#define SRENDER_PUBLIC_FIELDS(field) \
	field(srender, int, sprite) \
	field(srender, Vec3, color)

decl_component(srender, SRENDER_PUBLIC_FIELDS);

void srender_draw_all(Vec2);

#endif /* SRENDER_H */
