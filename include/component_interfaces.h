#ifndef COMPONENT_INTERFACES_H
#define COMPONENT_INTERFACES_H

#include <all_components.h>
#include <entity.h>

component_interface *get_component_interfaces(void);

void init_all_components(void);

void shutdown_all_components(void);

void start_all_components(void);

void flush_all_components(void);

#endif /* COMPONENT_INTERFACES_H */
