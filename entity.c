#include <entity.h>
#include <gamedef.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#define CREATE_ENTITY(i, g) (((uint64_t)(i) << 32)|((uint16_t)(g) << 16))

typedef struct {
	Entity id;
	bool enabled;
} EntityDesc;
static EntityDesc entities[MAX_ENTITY_COUNT];
static EntityIndex next_entity = 1;

void init_entities(void)
{
	for (int i = 0; i < MAX_ENTITY_COUNT - 1; i++) {
		entities[i] = (EntityDesc){
			CREATE_ENTITY(i+1, 0),
			false
		};
	}
	entities[MAX_ENTITY_COUNT-1] = (EntityDesc){
		CREATE_ENTITY(0, 0),
		false
	};
}

Entity create_entity(void)
{
	EntityDesc *entity;
	EntityIndex next_entity_p;
	EntityGeneration generation;
	EntityDesc new_entity;

	if (next_entity == 0 || next_entity == MAX_ENTITY_COUNT) return 0;
	entity = &entities[next_entity];
	next_entity_p = ENTITY_INDEX(entity->id);
	assert(next_entity_p != next_entity);

	generation = ENTITY_GENERATION(entities[next_entity].id);
	generation = (generation == MAX_ENTITY_GENERATION ?
		      0 : generation+1);
	new_entity = (EntityDesc){
		CREATE_ENTITY(next_entity, generation),
		true
	};
	*entity = new_entity;

	next_entity = next_entity_p;
	return new_entity.id;
}

bool is_entity_valid(Entity ent)
{
	if (ENTITY_INDEX(ent) >= MAX_ENTITY_COUNT) return false;
	return entities[ENTITY_INDEX(ent)].id == ent;
}

void destroy_entity(Entity ent)
{
	EntityDesc *entity;
	EntityIndex next_entity_p = next_entity;
	if (ENTITY_INDEX(ent) >= MAX_ENTITY_COUNT) return;
	entity = &entities[ENTITY_INDEX(ent)];
	if (entity->id != ent) return;

	next_entity = ENTITY_INDEX(entity->id);
	entity->id = CREATE_ENTITY(next_entity_p,
				   ENTITY_GENERATION(entity->id));
}
