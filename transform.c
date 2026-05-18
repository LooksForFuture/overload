#include <transform.h>
#include <entity.h>
#include <component.h>

static decl_component_storage(transform, TRANSFORM_FIELDS)

static inline void transform_init_private(void) {}
static inline void transform_add_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

static inline void transform_rem_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

Vec2 transform_position(transform handle)
{
	return transform_data.position[handle.id];
}

void transform_set_position(transform handle, Vec2 new_pos)
{
	transform_data.position[handle.id] = new_pos;
}

float transform_rotation(transform handle)
{
	return transform_data.rotation[handle.id];
}

void transform_set_rotation(transform handle, float new_rot)
{
	transform_data.rotation[handle.id] = new_rot;
}

Vec2 transform_scale(transform handle)
{
	return transform_data.scale[handle.id];
}

void transform_set_scale(transform handle, Vec2 new_scale)
{
	transform_data.scale[handle.id] = new_scale;
}
