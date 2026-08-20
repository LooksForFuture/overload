#include <intent.h>
#include <entity.h>
#include <component.h>

#define INTENT_FIELDS(field) \
	INTENT_PUBLIC_FIELDS(field)

static decl_component_storage(intent,
			      INTENT_PUBLIC_FIELDS,
			      INTENT_FIELDS)

static inline void intent_init_private(void) {}
static inline void intent_shutdown_private(void) {}

void intent_start(void) {}

static inline void intent_add_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

static inline void intent_rem_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

static inline Vec2 intent_movement_impl(EntityIndex id)
{
	return intent_data.movement[id];
}

static inline void intent_set_movement_impl(EntityIndex id, Vec2 new_mov)
{
	intent_data.movement[id] = new_mov;
}
