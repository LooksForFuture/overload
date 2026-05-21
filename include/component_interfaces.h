#ifndef COMPONENT_INTERFACES_H
#define COMPONENT_INTERFACES_H

#include <all_components.h>

#include <transform.h>
#include <srender.h>

typedef struct {
	void (*rem_entity_immediately)(Entity ent);
} component_interface;

#define X(name) {name##_rem__entity_immediately},
const component_interface component_interfaces[] = {
	ALL_COMPONENTS(X)
};
#undef X

#endif /* COMPONENT_INTERFACES_H */
