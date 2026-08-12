#include <srender.h>
#include <entity.h>
#include <component.h>
#include <r_main.h>
#include <gmath.h>

#include <transform.h>

static decl_component_storage(
	srender,
	SRENDER_PUBLIC_FIELDS,
	SRENDER_PUBLIC_FIELDS)

static inline void srender_init_private(void) {}
static inline void srender_shutdown_private(void) {}

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

static inline int srender_sprite_impl(EntityIndex id)
{
	return srender_data.sprite[id];
}

static inline void srender_set_sprite_impl(EntityIndex id, int new_sprite)
{
	srender_data.sprite[id] = new_sprite;
}

static inline Vec3 srender_color_impl(EntityIndex id)
{
	return srender_data.color[id];
}

static inline void srender_set_color_impl(EntityIndex id, Vec3 new_color)
{
	srender_data.color[id] = new_color;
}

void srender_draw_all(Vec2 camera)
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
		r_fill_rect(&(SDL_FRect){
				p.x + GAME_VIEW_WIDTH / 2 -
				s.x / 2 - camera.x,
				-p.y + GAME_VIEW_HEIGHT / 2 -
				s.y / 2 + camera.y,
				s.x, s.y});
	}
}
