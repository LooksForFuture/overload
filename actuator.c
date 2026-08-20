#include <actuator.h>
#include <entity.h>
#include <component.h>

#include <intent.h>
#include <collider.h>

#define ACTUATOR_FIELDS(field) \
	ACTUATOR_PUBLIC_FIELDS(field)

static decl_component_storage(actuator,
			      ACTUATOR_PUBLIC_FIELDS,
			      ACTUATOR_FIELDS)

static inline void actuator_init_private(void) {}
static inline void actuator_shutdown_private(void) {}

void actuator_start(void) {}

static inline void actuator_add_private(Entity ent, EntityIndex index)
{
	(void)ent;
	actuator_data.enabled[index] = true;
}

static inline void actuator_rem_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

void actuator_update(void)
{
	for (int ac = 1; ac <= actuator_data.count; ac++) {
		intent i;
		collider c;

		if (!actuator_data.enabled[ac]) continue;

		i = intent_get(actuator_data.rev_map[ac]);
		if (i.id == 0) continue;
		c = collider_get(actuator_data.rev_map[ac]);
		if (c.id == 0) continue;

		Vec2 movement = intent_movement(i);
		movement.x *= 2;
		movement.y *= 2;
		collider_set_velocity(c, movement);
	}
}


static inline bool
actuator_enabled_impl(EntityIndex id)
{
	return actuator_data.enabled[id];
}

static inline void
actuator_set_enabled_impl(EntityIndex id, bool enabled)
{
	actuator_data.enabled[id] = enabled;
}
