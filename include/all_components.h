#ifndef ALL_COMPONENTS_H
#define ALL_COMPONENTS_H

#define ALL_COMPONENTS(F) \
	F(transform) \
	F(srender) \
	F(collider) \
	F(intent) \
	F(actuator)

#define X(name) COMPONENT_##name,
enum {
	ALL_COMPONENTS(X)
	COMPONENT_COUNT,
};
#undef X

#endif /* ALL_COMPONENTS_H */
