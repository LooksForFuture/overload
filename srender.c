#include <srender.h>
#include <entity.h>
#include <component.h>
#include <r_main.h>

#include <transform.h>

static decl_component_storage(srender, SRENDER_FIELDS)

static inline void srender_init_private(void) {}

static inline void srender_add_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

static inline void srender_rem_private(Entity ent, EntityIndex index)
{
	(void)ent;
	(void)index;
}

int srender_sprite(srender handle)
{
	return srender_data.sprite[handle.id];
}

void srender_set_sprite(srender handle, int new_sprite)
{
	srender_data.sprite[handle.id] = new_sprite;
}

Vec3 srender_color(srender handle)
{
	return srender_data.color[handle.id];
}

void srender_set_color(srender handle, Vec3 new_color)
{
	srender_data.color[handle.id] = new_color;
}

void srender_draw_all(void)
{
	for (int i = 1; i <= srender_data.count; i++) {
		transform t;
		Vec3 color;
		Vec2 p, s;

		t = transform_get(srender_data.rev_map[i]);
		if (t.id == 0) continue;

		color = srender_data.color[i];
		p = transform_position(t);
		s = transform_scale(t);
		r_set_draw_color(color.x, color.y ,color.z, 255);
		r_fill_rect(&(SDL_FRect){p.x, p.y, s.x, s.y});
	}
}
