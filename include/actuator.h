#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <component.h>
#include <stdbool.h>

#define ACTUATOR_PUBLIC_FIELDS(field) \
	field(actuator, bool, enabled)

decl_component(actuator, ACTUATOR_PUBLIC_FIELDS);

void actuator_update(void);

#endif /* ACTUATOR_H */
