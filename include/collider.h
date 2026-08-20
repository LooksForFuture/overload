#ifndef COLLIDER_H
#define COLLIDER_H

#include <component.h>
#include <gmath.h>

typedef struct {
	Entity a;
	Entity b;
	Vec2 normal;
	float penetration;
} Collision;

#define COLLIDER_PUBLIC_FIELDS(field) \
	field(collider, Vec2, velocity) \
	field(collider, float, radius)

decl_component(collider, COLLIDER_PUBLIC_FIELDS);

void collider_sub_enter(void (*on_collision_enter)(Collision));

void collider_sub_stay(void (*on_collision_stay)(Collision));

void collider_sub_exit(void (*on_collision_exit)(Collision));

void collider_sync_physics(void);

#endif /* COLLIDER_H */
