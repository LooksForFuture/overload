#ifndef COMPONENT_INTERFACES_H
#define COMPONENT_INTERFACES_H

#include <all_components.h>
#include <entity.h>

typedef struct {
	void (*rem_entity_immediately)(Entity ent);
	void (*inspector_default)(Entity ent);
} component_interface;

const component_interface *get_component_interfaces(void);

void init_all_components(void);

void shutdown_all_components(void);

void flush_all_components(void);

#endif /* COMPONENT_INTERFACES_H */
