#ifndef COMPONENT_INTERFACES_H
#define COMPONENT_INTERFACES_H

#include <all_components.h>

#include <transform.h>
#include <srender.h>

typedef struct {
	void (*rem_entity)(Entity ent);
} component_interface;

#define X(name) {name##_rem_entity},
const component_interface component_interfaces[] = {
	ALL_COMPONENTS(X)
};
#undef X

#endif /* COMPONENT_INTERFACES_H */
