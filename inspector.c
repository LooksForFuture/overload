#include <inspector.h>

#ifndef NO_EDITOR

#include <all_components.h>
#include <entity.h>
#include <component_interfaces.h>
#include <editor.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void inspector_inspect(Entity entity)
{
	uint64_t entity_mask = 0;
	const component_interface *interfaces =
		get_component_interfaces();

	if (!is_entity_valid(entity)) return;
	entity_mask = get_entity_component_mask(entity);
	if (editor_begin("Inspector", &(SDL_Rect){30, 30, 250, 250})) {
		editor_layout_row_dynamic(0, 1);
		if (editor_button("destroy")) destroy_entity(entity);
		if (entity_mask != 0) {
			for (int i = 0; i < COMPONENT_COUNT; i++) {
				uint64_t mask = 1ULL<<i;
				if ((entity_mask & mask) == mask) {
					interfaces[i].
						inspector_default(entity);
				}
			}
		}
	} editor_end();
}

void Vec2_inspector_widget(EntityIndex index, const Vec2 *v,
			   void (*setter)(EntityIndex, Vec2))
{
	float new_x;
	float new_y;
	editor_layout_row_dynamic(0, 2);
	new_x = editor_propertyf("#x", FLT_MIN, v->x, FLT_MAX);
	new_y = editor_propertyf("#y", FLT_MIN, v->y, FLT_MAX);

	if (new_x != v->x || new_y != v->y) {
		Vec2 new_vec = (Vec2){new_x, new_y};
		setter(index, new_vec);
	}
}

void Vec3_inspector_widget(EntityIndex index, const Vec3 *v,
			   void (*setter)(EntityIndex, Vec3))
{

	float new_x;
	float new_y;
	float new_z;
	editor_layout_row_dynamic(0, 3);
	new_x = editor_propertyf("#x", FLT_MIN, v->x, FLT_MAX);
	new_y = editor_propertyf("#y", FLT_MIN, v->y, FLT_MAX);
	new_z = editor_propertyf("#z", FLT_MIN, v->z, FLT_MAX);

	if (new_x != v->x || new_y != v->y || new_z != v->z) {
		Vec3 new_vec = (Vec3){new_x, new_y, new_x};
		setter(index, new_vec);
	}
}

void bool_inspector_widget(EntityIndex index, const bool *b,
			   void (*setter)(EntityIndex, bool))
{
	(void)index;
	(void)setter;
	editor_layout_row_dynamic(0, 1);
	editor_label(*b ? "true": "false", ED_ALIGN_CENTER);
}

void float_inspector_widget(EntityIndex index, const float *f,
			    void (*setter)(EntityIndex, float))
{
	float new_f;
	editor_layout_row_dynamic(0, 1);
	new_f = editor_propertyf("#", FLT_MIN, *f, FLT_MAX);
	if (new_f != *f) {
		setter(index, new_f);
	}
}

void int_inspector_widget(EntityIndex index, const int *i,
			  void (*setter)(EntityIndex, int))
{
	int new_i;
	editor_layout_row_dynamic(0, 1);
	new_i = editor_propertyi("#", INT_MIN, *i, INT_MAX);
	if (new_i != *i) {
		setter(index, new_i);
	}
}

#else

void editor_dummy() {}

#endif /* NO_EDITOR */
