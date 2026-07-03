#ifndef COLLIDER_H
#define COLLIDER_H

#include <component.h>
#include <gmath.h>

#define COLLIDER_PUBLIC_FIELDS(field) \
	field(collider, Vec2, velocity)

decl_component(collider, COLLIDER_PUBLIC_FIELDS);

#endif /* COLLIDER_H */
