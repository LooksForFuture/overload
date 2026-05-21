#ifndef COMPONENT_H
#define COMPONENT_H

#include <gamedef.h>
#include <entity.h>
#include <all_components.h>
#include <dsa.h>

#include <stddef.h>

#define component_getset_fields(component, type, name) \
	type component##_##name(component); \
	void component##_set_##name(component, type)

#define component_storage_fields(component, type, name) \
	type name[MAX_ENTITY_COUNT]

#define decl_component(name, fields_f) \
	typedef struct { EntityIndex id; } name; \
	\
	void name##_init(void); \
	void name##_shutdown(void); \
	name name##_add(Entity); \
	name name##_get(Entity); \
	void name##_rem_entity(Entity); \
	void name##_rem__entity_immediately(Entity); \
	void name##_flush_entities(void); \
	fields_f(component_getset_fields)

#define component_copy_last_to_index(component, type, name) \
	component##_data.name[index] = \
		component##_data.name[component##_data.count]

#define decl_component_storage(name, fields_f) \
	struct { \
		int count; \
		struct { EntityIndex *items; size_t count; \
			size_t capacity; } rem_pending; \
		component_storage_fields(name, EntityIndex, map); \
		component_storage_fields(name, Entity, rev_map); \
		component_storage_fields(name, bool, pending_remove); \
		fields_f(component_storage_fields); \
	} name##_data; \
	\
	static inline void name##_init_private(void); \
	void name##_init(void) { \
		for (int i = 0; i < MAX_ENTITY_COUNT; i++) { \
			name##_data.map[i] = 0; \
			name##_data.rev_map[i] = 0; \
		} \
		name##_data.count = 0; \
		name##_init_private(); \
		name##_data.rem_pending.items = 0; \
		name##_data.rem_pending.count = 0; \
		name##_data.rem_pending.capacity = 0; \
	} \
	static inline void name##_shutdown_private(void); \
	void name##_shutdown(void) { \
		name##_shutdown_private(); \
	} \
	\
	static inline void name##_add_private(Entity, EntityIndex); \
	name name##_add(Entity ent) { \
		EntityIndex ent_index; \
		EntityIndex index; \
		if (!is_entity_valid(ent)) return (name){0}; \
		ent_index = ENTITY_INDEX(ent); \
		index = name##_data.map[ent_index]; \
		if (index != 0) return (name){ index }; \
		index = ++name##_data.count; \
		name##_data.map[ent_index] = index; \
		name##_data.rev_map[index] = ent; \
		name##_data.pending_remove[index] = false; \
		component_added_to_entity(ent, 1<<COMPONENT_##name);\
		name##_add_private(ent, index); \
		return (name){index}; \
	} \
	\
	name name##_get(Entity ent) { \
		name handle = {0}; \
		if (!is_entity_valid(ent)) return handle; \
		handle.id = name##_data.map[ENTITY_INDEX(ent)]; \
		return handle; \
	} \
	\
	static inline void name##_rem_private(Entity, EntityIndex); \
	void name##_rem_entity(Entity ent) { \
		EntityIndex index = 0; \
		if (!is_entity_valid(ent)) return; \
		index = name##_data.map[ENTITY_INDEX(ent)]; \
		if (index == 0 || \
		    name##_data.pending_remove[index]) return;	\
		\
		da_append(&name##_data.rem_pending, index); \
		name##_data.pending_remove[index] = true; \
	} \
	\
	static void name##_rem__from_index_immediate( \
		Entity ent, \
		EntityIndex index) { \
		name##_rem_private(ent, index); \
		fields_f(component_copy_last_to_index); \
		component_copy_last_to_index(name, Entity, rev_map); \
		name##_data.map[ENTITY_INDEX(ent)] = 0; \
		name##_data.map[ENTITY_INDEX( \
				name##_data.rev_map[ \
					name##_data.count] \
				)] = index; \
		name##_data.count--; \
		component_removed_from_entity( \
			ent, 1<<COMPONENT_##name); \
	} \
	\
	void name##_rem__entity_immediately(Entity ent) { \
		EntityIndex index = 0; \
		index = name##_data.map[ENTITY_INDEX(ent)]; \
		name##_rem__from_index_immediate(ent, index); \
	} \
	\
	void name##_flush_entities(void) { \
		if (name##_data.rem_pending.count == 0) return; \
		da_foreach(EntityIndex, index_p, \
			   &name##_data.rem_pending) {	\
			EntityIndex index = *index_p; \
			Entity ent = name##_data.rev_map[index]; \
			if (!is_entity_valid(ent)) return; \
			name##_rem__from_index_immediate(ent, index); \
		} \
		name##_data.rem_pending.count = 0; \
	}

#endif /* COMPONENT_H */
