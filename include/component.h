#ifndef COMPONENT_H
#define COMPONENT_H

#include <gamedef.h>

#define Public(gen, _) gen
#define Private(_, gen) gen

#define component_getset_fields(component, type, name, access_f) \
	type component##_##name(component); \
	access_f(void component##_set_##name(component, type),)

#define component_storage_fields(component, type, name, access_f) \
	type name[MAX_ENTITY_COUNT]

#define decl_component(name, fields_f) \
	typedef struct { EntityIndex id; } name; \
	\
	void name##_init(void); \
	name name##_add(Entity); \
	name name##_get(Entity); \
	fields_f(component_getset_fields)

#define decl_component_storage(name, fields_f) \
	struct { \
		int count; \
		EntityIndex rev_map[MAX_ENTITY_COUNT]; \
		fields_f(component_storage_fields); \
	} name##_data; \
	\
	inline void name##_init_private(void); \
	void name##_init(void) { \
		for (int i = 0; i < MAX_ENTITY_COUNT; i++) { \
			name##_data.rev_map[i] = 0; \
		} \
		name##_data.count = 0; \
		name##_init_private(); \
	} \
	\
	name name##_add(Entity ent) { \
		EntityIndex index; \
		EntityIndex *index_p; \
		if (!is_entity_valid(ent)) return (name){0}; \
		index_p = &name##_data.rev_map[ENTITY_INDEX(ent)]; \
		index = *index_p; \
		if (index != 0) return (name){ index }; \
		index = ++name##_data.count; \
		*index_p = index; \
		return (name){index}; \
	} \
	\
	name name##_get(Entity ent) { \
		name handle = {0}; \
		if (!is_entity_valid(ent)) return handle; \
		handle.id = name##_data.rev_map[ENTITY_INDEX(ent)]; \
		return handle; \
	}

#endif /* COMPONENT_H */
