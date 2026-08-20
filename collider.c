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

static void (*on_collision_enters[64])(Collision);
static int on_enter_count = 0;

static void (*on_collision_stays[64])(Collision);
static int on_stay_count = 0;

static void (*on_collision_exits[64])(Collision);
static int on_exit_count = 0;

static void collision_listener(phCollisionEvent event)
{
	Collision col = (Collision) {
		ph_get_body_user_data(event.body_a),
		ph_get_body_user_data(event.body_b),
		event.normal,
		event.penetration
	};
	switch (event.type) {
	case PH_COLLISION_ENTER:
		for (int i = 0; i < on_enter_count; i++) {
			on_collision_enters[i](col);
		}
		break;
	case PH_COLLISION_STAY:
		for (int i = 0; i < on_stay_count; i++) {
			on_collision_stays[i](col);
		}
		break;
	case PH_COLLISION_EXIT:
		for (int i = 0; i < on_exit_count; i++) {
			on_collision_exits[i](col);
		}
		break;
	}
}

static inline void collider_init_private(void) {}
static inline void collider_shutdown_private(void) {}

void collider_start(void) {}

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
	ph_destroy_body(collider_data.body[index]);
}

void collider_sub_enter(void (*on_collision_enter)(Collision))
{
	on_collision_enters[on_enter_count++] = on_collision_enter;
}

void collider_sub_stay(void (*on_collision_stay)(Collision))
{
	on_collision_stays[on_stay_count++] = on_collision_stay;
}

void collider_sub_exit(void (*on_collision_exit)(Collision))
{
	on_collision_exits[on_exit_count++] = on_collision_exit;
}

void collider_sync_physics(void)
{
	int event_count;
	const phCollisionEvent *events;

	for (int i = 1; i <= collider_data.count; i++) {
		transform t = transform_get(collider_data.rev_map[i]);
		if (t.id == 0) continue;

		Vec2 position = ph_get_body_position(collider_data.body[i]);
		transform_set_position(t, position);
	}

	events = ph_get_events(&event_count);
	for (int i = 0; i < event_count; i++) {
		collision_listener(events[i]);
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
