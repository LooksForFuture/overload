#include <actuator.h>
#include <entity.h>
#include <component.h>

#define ACTUATOR_FIELDS(field) \
	ACTUATOR_PUBLIC_FIELDS(field)

static decl_component_storage(actuator,
			      ACTUATOR_PUBLIC_FIELDS,
			      ACTUATOR_FIELDS)

static inline void actuator_init_private(void) {}
static inline void actuator_shutdown_private(void) {}
static inline void actuator_add_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

static inline void actuator_rem_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}
