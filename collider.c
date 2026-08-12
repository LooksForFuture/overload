#include <collider.h>

#include <physics.h>

#include <entity.h>
#include <component.h>
#include <transform.h>

#define COLLIDER_PRIVATE_FIELDS(field) \
	COLLIDER_PUBLIC_FIELDS(field) \
	field(collider, phBody, body)

static decl_component_storage(collider,
			      COLLIDER_PUBLIC_FIELDS,
			      COLLIDER_PRIVATE_FIELDS)

static inline void collider_init_private(void) {}
static inline void collider_shutdown_private(void) {}
static inline void collider_add_private(Entity ent, EntityIndex index)
{
	transform t = transform_get(ent);
	phBody body = ph_new_body(PH_KINEMATIC);
	if (body == 0) return;
	collider_data.body[index] = body;
	if (t.id != 0) {
		ph_set_body_position(body, transform_position(t));
		collider_set_radius_impl(index,transform_scale(t).x/2);
	}
	ph_set_body_user_data(body, ent);
}

static inline void collider_rem_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

void collider_update_physics(float dt)
{
	ph_update(dt);
	for (int i = 1; i <= collider_data.count; i++) {
		transform t = transform_get(collider_data.rev_map[i]);
		if (t.id == 0) continue;

		Vec2 position = ph_get_body_position(collider_data.body[i]);
		transform_set_position(t, position);
	}
}

static inline Vec2
collider_velocity_impl(EntityIndex id)
{
	return collider_data.velocity[id];
}

static inline void
collider_set_velocity_impl(EntityIndex id, Vec2 new_vel)
{
	collider_data.velocity[id] = new_vel;
	ph_set_body_velocity(collider_data.body[id], new_vel);
}

static inline float
collider_radius_impl(EntityIndex id)
{
	return collider_data.radius[id];
}

static inline void
collider_set_radius_impl(EntityIndex id, float new_rad)
{
	collider_data.radius[id] = new_rad;
	ph_set_body_radius(collider_data.body[id], new_rad);
}
