#include <transform.h>
#include <entity.h>
#include <component.h>

static decl_component_storage(transform, TRANSFORM_FIELDS)

void transform_init_private(void) {}

Vec2 transform_position(transform handle)
{
	return transform_data.position[handle.id-1];
}

void transform_set_position(transform handle, Vec2 new_pos)
{
	transform_data.position[handle.id-1] = new_pos;
}

float transform_rotation(transform handle)
{
	return transform_data.rotation[handle.id-1];
}

void transform_set_rotation(transform handle, float new_rot)
{
	transform_data.rotation[handle.id-1] = new_rot;
}

Vec2 transform_scale(transform handle)
{
	return transform_data.scale[handle.id-1];
}

void transform_set_scale(transform handle, Vec2 new_scale)
{
	transform_data.scale[handle.id-1] = new_scale;
}
