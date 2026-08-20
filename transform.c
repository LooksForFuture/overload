#include <transform.h>
#include <entity.h>
#include <component.h>

static decl_component_storage(transform,
			      TRANSFORM_PUBLIC_FIELDS,
			      TRANSFORM_PUBLIC_FIELDS)

static inline void transform_init_private(void) {}
static inline void transform_shutdown_private(void) {}

void transform_start(void) {}

static inline void transform_add_private(Entity ent, EntityIndex index)
{
	(void)ent;
	transform_data.position[index] = (Vec2){0.0f, 0.0f};
	transform_data.rotation[index] = 0.0f;
	transform_data.scale[index] = (Vec2){1.0f, 1.0f};
}

static inline void transform_rem_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

static inline Vec2 transform_position_impl(EntityIndex id)
{
	return transform_data.position[id];
}

static inline void
transform_set_position_impl(EntityIndex id, Vec2 new_pos)
{
	transform_data.position[id] = new_pos;
}

static inline float transform_rotation_impl(EntityIndex id)
{
	return transform_data.rotation[id];
}

static inline void
transform_set_rotation_impl(EntityIndex id, float new_rot)
{
	transform_data.rotation[id] = new_rot;
}

static inline Vec2 transform_scale_impl(EntityIndex id)
{
	return transform_data.scale[id];
}

static inline void
transform_set_scale_impl(EntityIndex id, Vec2 new_scale)
{
	transform_data.scale[id] = new_scale;
}
