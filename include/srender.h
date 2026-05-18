#ifndef SRENDER_H
#define SRENDER_H

#include <transform.h>

#define SRENDER_FIELDS(field) \
	field(srender, int, sprite); \
	field(srender, Vec3, color)

decl_component(srender, SRENDER_FIELDS);

void srender_draw_all(void);

#endif /* SRENDER_H */
