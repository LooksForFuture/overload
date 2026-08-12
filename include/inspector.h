#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <stdbool.h>
#include <gmath.h>
#include <entity.h>

#ifndef NO_EDITOR
void inspector_inspect(Entity);

void Vec2_inspector_widget(EntityIndex, const Vec2 *,
			   void (*setter)(EntityIndex, Vec2));
void Vec3_inspector_widget(EntityIndex, const Vec3 *,
			   void (*setter)(EntityIndex, Vec3));
void bool_inspector_widget(EntityIndex, const bool *,
			   void (*setter)(EntityIndex, bool));
void float_inspector_widget(EntityIndex, const float *,
			    void (*setter)(EntityIndex, float));
void int_inspector_widget(EntityIndex, const int *,
			    void (*setter)(EntityIndex, int));
#endif

#endif /* INSPECTOR_H */
