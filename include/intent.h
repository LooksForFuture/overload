#ifndef INTENT_H
#define INTENT_H

#include <component.h>
#include <gmath.h>

#define INTENT_PUBLIC_FIELDS(field) \
	field(intent, Vec2, movement)

decl_component(intent, INTENT_PUBLIC_FIELDS);

#endif /* INTENT_H */
