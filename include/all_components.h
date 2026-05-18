#ifndef ALL_COMPONENTS_H
#define ALL_COMPONENTS_H

#include <entity.h>

#define ALL_COMPONENTS(F) \
	F(transform) \
	F(srender)

#define X(name) COMPONENT_##name,
enum {
	ALL_COMPONENTS(X)
	COMPONENT_COUNT,
};
#undef X

#endif /* ALL_COMPONENTS_H */
