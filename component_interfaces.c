#include <component_interfaces.h>

#include <transform.h>
#include <srender.h>
#include <collider.h>
#include <intent.h>
#include <actuator.h>

#define X(name) {name##_rem_entity_immediately, \
			name##_inspector_default},
const component_interface component_interfaces[] = {
	ALL_COMPONENTS(X)
};
#undef X

const component_interface *get_component_interfaces(void)
{
	return component_interfaces;
}

#define X(name) name##_init();
void init_all_components(void)
{
	ALL_COMPONENTS(X)
}
#undef X

#define X(name) name##_shutdown();
void shutdown_all_components(void)
{
	ALL_COMPONENTS(X)
}
#undef X

#define X(name) name##_flush_entities();
void flush_all_components(void)
{
	ALL_COMPONENTS(X)
}
#undef X
