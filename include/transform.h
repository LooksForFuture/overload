#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <component.h>
#include <entity.h>

typedef struct {
	float x, y;
} Vec2;

typedef struct {
	float x, y, z;
} Vec3;

#define TRANSFORM_FIELDS(field) \
	field(transform, Vec2, position); \
	field(transform, float, rotation); \
	field(transform, Vec2, scale)

decl_component(transform, TRANSFORM_FIELDS);

#endif /* TRANSFORM_H */
