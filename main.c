#include <gamedef.h>
#include <component.h>

#include <entity.h>
#include <transform.h>

#include <stdio.h>

int main(void)
{
	Entity ent1, ent2, ent3;
	init_entities();
	ent1 = create_entity();
	printf("%u\n", ENTITY_INDEX(ent1));
	ent2 = create_entity();
	printf("%u\n", ENTITY_INDEX(ent2));
	destroy_entity(ent1);
	printf("%b\n", is_entity_valid(ent1));
	ent1 = create_entity();
	printf("%u\n%b\n", ENTITY_INDEX(ent1), is_entity_valid(ent1));
	ent3 = create_entity();
	printf("%u\n", ENTITY_INDEX(ent3));
	return 0;
}
