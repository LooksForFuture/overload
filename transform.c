#include <transform.h>
#include <component.h>

static decl_component_storage(transform, TRANSFORM_FIELDS);

Vec2 transform_read_position(void)
{
	return transform.position[0];
}
