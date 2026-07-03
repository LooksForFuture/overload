#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <stdbool.h>
#include <gmath.h>
#include <entity.h>

#ifndef NO_EDITOR
void inspector_inspect(Entity);

void Vec2_inspector_widget(const Vec2 *);
void Vec3_inspector_widget(const Vec3 *);
void bool_inspector_widget(const bool *);
void float_inspector_widget(const float *);
void int_inspector_widget(const int *);
#endif

#endif /* INSPECTOR_H */
