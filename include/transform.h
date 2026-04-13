#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <component.h>

typedef struct {
	float x, y, z;
} Vec2;

#define TRANSFORM_FIELDS(Field) \
	Field(transform, Vec2, position, ReadWrite); \
	Field(transform, Vec2, scale, ReadWrite); \
	Field(transform, float, rotation, ReadWrite) \

decl_component(transform, TRANSFORM_FIELDS);

#endif /* TRANSFORM_H */
