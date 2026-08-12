#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <component.h>
#include <gmath.h>

#define TRANSFORM_PUBLIC_FIELDS(field) \
	field(transform, Vec2, position) \
	field(transform, float, rotation) \
	field(transform, Vec2, scale)

decl_component(transform, TRANSFORM_PUBLIC_FIELDS);

#endif /* TRANSFORM_H */
