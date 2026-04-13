#ifndef COMPONENT_H
#define COMPONENT_H

#include <gamedef.h>

#define ReadWrite(gen, _) gen
#define ReadOnly(_, gen) gen
#define Private(_, __)

#define component_struct_fields(component, type, name, access_f) \
	access_f(\
		type name, const type name \
		)

#define component_getset_fields(component, type, name, access_f) \
	type component##_read_##name(void); \
	access_f(void component##_write_##name(type),)

#define component_storage_fields(component, type, name, access_f) \
	type name[MAX_ENTITY_COUNT]

#define decl_component(name, fields_f) typedef struct { \
		fields_f(component_struct_fields); \
	} name##View;			 \
	\
	fields_f(component_getset_fields)

#define decl_component_storage(name, fields_f) \
	struct { \
		fields_f(component_storage_fields); \
	} name

#endif /* COMPONENT_H */
