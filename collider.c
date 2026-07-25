#include <collider.h>
#include <entity.h>
#include <component.h>
#include <transform.h>

#include <stdio.h>

static decl_component_storage(collider,
			      COLLIDER_PUBLIC_FIELDS,
			      COLLIDER_PUBLIC_FIELDS)

static inline void collider_init_private(void) {}
static inline void collider_shutdown_private(void) {}
static inline void collider_add_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

static inline void collider_rem_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

void collider_update_physics(float dt)
{
	for (int i = 1; i <= collider_data.count+1; i++) {
		transform t = transform_get(collider_data.rev_map[i]);
		if (t.id == 0) return;

		Vec2 position = transform_position(t);
		Vec2 velocity = collider_data.velocity[i];
		position.x += velocity.x * dt;
		position.y += velocity.y * dt;
		transform_set_position(t, position);
	}
}

Vec2 collider_velocity(collider handle)
{
	return collider_data.velocity[handle.id];
}

void collider_set_velocity(collider handle, Vec2 new_vel)
{
	collider_data.velocity[handle.id] = new_vel;
}
