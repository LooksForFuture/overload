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

void Vec2_inspector_widget(const Vec2 *v)
{
	char buf[32];
	editor_layout_row_dynamic(0, 4);
	editor_label("x:", ED_ALIGN_CENTER);
	snprintf(buf, sizeof(buf), "%.3f", v->x);
	editor_label(buf, ED_ALIGN_CENTER);
	editor_label("y:", ED_ALIGN_CENTER);
	snprintf(buf, sizeof(buf), "%.3f", v->y);
	editor_label(buf, ED_ALIGN_CENTER);
}

void Vec3_inspector_widget(const Vec3 *v)
{
	char buf[32];
	editor_layout_row_dynamic(0, 6);
	editor_label("x:", ED_ALIGN_CENTER);
	snprintf(buf, sizeof(buf), "%.3f", v->x);
	editor_label(buf, ED_ALIGN_CENTER);
	editor_label("y:", ED_ALIGN_CENTER);
	snprintf(buf, sizeof(buf), "%.3f", v->y);
	editor_label(buf, ED_ALIGN_CENTER);
	editor_label("z:", ED_ALIGN_CENTER);
	snprintf(buf, sizeof(buf), "%.3f", v->z);
	editor_label(buf, ED_ALIGN_CENTER);
}

void bool_inspector_widget(const bool *b)
{
	editor_layout_row_dynamic(0, 1);
	editor_label(*b ? "true": "false", ED_ALIGN_CENTER);
}

void float_inspector_widget(const float *f)
{
	char buf[32];
	editor_layout_row_dynamic(0, 1);
	snprintf(buf, sizeof(buf), "%f", *f);
	editor_label(buf, ED_ALIGN_CENTER);
}

void int_inspector_widget(const int *i)
{
	char buf[32];
	editor_layout_row_dynamic(0, 1);
	snprintf(buf, sizeof(buf), "%d", *i);
	editor_label(buf, ED_ALIGN_CENTER);
}

#else

void editor_dummy() {}

#endif /* NO_EDITOR */
