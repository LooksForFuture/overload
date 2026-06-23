#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <entity.h>

typedef struct {
	float x, y;
} Vec2;

typedef struct {
	float x, y, z;
} Vec3;

#ifndef NO_EDITOR
void inspector_inspect(Entity);

void Vec2_inspector_widget(const Vec2 *);
void Vec3_inspector_widget(const Vec3 *);
void float_inspector_widget(const float *);
void int_inspector_widget(const int *);
#endif

#endif /* INSPECTOR_H */
