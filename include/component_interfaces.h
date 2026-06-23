#ifndef COMPONENT_INTERFACES_H
#define COMPONENT_INTERFACES_H

#include <all_components.h>

#include <transform.h>
#include <srender.h>

typedef struct {
	void (*rem_entity_immediately)(Entity ent);
	void (*inspector_default)(Entity ent);
} component_interface;

const component_interface *get_component_interfaces(void);

#endif /* COMPONENT_INTERFACES_H */
