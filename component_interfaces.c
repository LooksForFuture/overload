#include <component_interfaces.h>

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
