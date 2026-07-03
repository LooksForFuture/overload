#include <collider.h>
#include <entity.h>
#include <component.h>

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

Vec2 collider_velocity(collider handle)
{
	return collider_data.velocity[handle.id];
}

void collider_set_velocity(collider handle, Vec2 new_vel)
{
	collider_data.velocity[handle.id] = new_vel;
}
