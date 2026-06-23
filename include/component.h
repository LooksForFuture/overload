#ifndef COMPONENT_H
#define COMPONENT_H

#include <gamedef.h>
#include <entity.h>
#include <all_components.h>
#include <dsa.h>

#ifndef NO_EDITOR
#include <editor.h>
#include <inspector.h>
#endif

#include <stddef.h>

#ifndef NO_EDITOR

#define def_component_inspector(name) void \
	name##_inspector_default(Entity)

#define decl_component_inspector(name, public_fields) \
	void name##_inspector_default(Entity ent) { \
		if (editor_tree_push(ED_TREE_NODE, \
				     #name, ED_MINIMIZED)) { \
			name handle = name##_get(ent); \
			editor_layout_row_dynamic(0, 1); \
			editor_label(#name, ED_ALIGN_LEFT); \
			public_fields(component_field_inspector); \
			editor_tree_pop(); \
		} \
	}
#else

#define def_component_inspector(name)

#define decl_component_inspector(name, public_fields)

#endif /* NO_EDITOR */

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
	void name##_rem_entity_immediately(Entity); \
	void name##_flush_entities(void); \
	void name##_inspector_default(Entity);	\
	fields_f(component_getset_fields); \
	def_component_inspector(name)

#define component_copy_last_to_index(component, type, name) \
	component##_data.name[index] = \
		component##_data.name[component##_data.count]


#define component_field_inspector(component, type, name) { \
		editor_layout_row_dynamic(0, 2); \
		editor_label(#name":", ED_ALIGN_RIGHT); \
		editor_label(#type, ED_ALIGN_LEFT); \
		const type *p = &component##_data.name[handle.id]; \
		type##_inspector_widget(p); \
	}

#define decl_component_storage(name, public_fields, fields_f) \
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
		glut_free(name##_data.rem_pending.items); \
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
		component_added_to_entity(ent, 1ULL<<COMPONENT_##name);\
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
	static void name##_rem_from_index_immediate( \
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
			ent, 1ULL<<COMPONENT_##name); \
	} \
	\
	void name##_rem_entity_immediately(Entity ent) { \
		EntityIndex index = 0; \
		index = name##_data.map[ENTITY_INDEX(ent)]; \
		name##_rem_from_index_immediate(ent, index); \
	} \
	\
	void name##_flush_entities(void) { \
		if (name##_data.rem_pending.count == 0) return; \
		da_foreach(EntityIndex, index_p, \
			   &name##_data.rem_pending) {	\
			EntityIndex index = *index_p; \
			Entity ent = name##_data.rev_map[index]; \
			if (!is_entity_valid(ent)) return; \
			name##_rem_from_index_immediate(ent, index); \
		} \
		name##_data.rem_pending.count = 0; \
	} \
	decl_component_inspector(name, public_fields)

#endif /* COMPONENT_H */
