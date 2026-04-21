#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <component.h>
#include <entity.h>

typedef struct {
	float x, y;
} Vec2;

#define TRANSFORM_FIELDS(field) \
	field(transform, Vec2, position, Public); \
	field(transform, float, rotation, Public); \
	field(transform, Vec2, scale, Public)

decl_component(transform, TRANSFORM_FIELDS);

#endif /* TRANSFORM_H */
