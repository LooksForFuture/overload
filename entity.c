#include <entity.h>
#include <gamedef.h>
#include <dsa.h>
#include <all_components.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define CREATE_ENTITY(i, g) (((uint64_t)(i) << 32)|((uint16_t)(g) << 16))

typedef struct {
	Entity id;
	bool enabled;
	bool pending;
	uint64_t mask;
} EntityDesc;
static EntityDesc entities[MAX_ENTITY_COUNT];
static EntityIndex next_entity = 1;

static struct {
	Entity *items;
	size_t count;
	size_t capacity;
} destroy_pending; /* entities pending to be destroyed */

static const component_interface *interfaces = NULL;

void init_entities(void)
{
	for (int i = 0; i < MAX_ENTITY_COUNT - 1; i++) {
		entities[i] = (EntityDesc){
			CREATE_ENTITY(i+1, 0),
			false,
			false,
			0
		};
	}
	entities[MAX_ENTITY_COUNT-1] = (EntityDesc){
		CREATE_ENTITY(0, 0),
		false,
		false,
		0
	};

	destroy_pending.items = NULL;
	destroy_pending.count = 0;
	destroy_pending.capacity = 0;
}

void shutdown_entities(void)
{
	glut_free(destroy_pending.items);
}

void set_entity_component_interfaces(const component_interface *p)
{
	interfaces = p;
}

Entity create_entity(void)
{
	EntityDesc *entity;
	EntityIndex next_entity_p;
	EntityGeneration generation;
	EntityDesc new_entity;

	if (next_entity==0 || next_entity==MAX_ENTITY_COUNT) return 0;
	entity = &entities[next_entity];
	next_entity_p = ENTITY_INDEX(entity->id);
	assert(next_entity_p != next_entity);

	generation = ENTITY_GENERATION(entities[next_entity].id);
	generation = (generation == MAX_ENTITY_GENERATION ?
		      0 : generation+1);
	new_entity = (EntityDesc){
		CREATE_ENTITY(next_entity, generation),
		true,
		false,
		0
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

void component_added_to_entity(Entity ent, uint64_t id)
{
	EntityDesc *entity;
	if (ENTITY_INDEX(ent) >= MAX_ENTITY_COUNT) return;
	entity = &entities[ENTITY_INDEX(ent)];
	if (entity->id != ent) return;

	entity->mask = entity-> mask | id;
}

void component_removed_from_entity(Entity ent, uint64_t id)
{
	EntityDesc *entity;
	if (ENTITY_INDEX(ent) >= MAX_ENTITY_COUNT) return;
	entity = &entities[ENTITY_INDEX(ent)];
	if (entity->id != ent) return;

	entity->mask = entity-> mask & (~id);
}

void destroy_entity(Entity ent)
{
	EntityDesc *entity;
	/* EntityIndex next_entity_p = next_entity; */
	if (ENTITY_INDEX(ent) >= MAX_ENTITY_COUNT) return;
	entity = &entities[ENTITY_INDEX(ent)];
	if (entity->id != ent || entity->pending) return;

	da_append(&destroy_pending, ent);
	entity->pending = true;

	/* if (entity->mask != 0) {
		for (int i = 0; i < COMPONENT_COUNT; i++) {
			uint64_t mask = pow(2, i);
			if ((entity->mask & mask) == mask) {
				component_interfaces[i].rem_entity(ent);
			}
		}
	}

	next_entity = ENTITY_INDEX(entity->id);
	entity->id = CREATE_ENTITY(next_entity_p,
	ENTITY_GENERATION(entity->id)); */
}

static inline void destroy_entity_immediately(Entity ent)
{
	EntityDesc *entity = &entities[ENTITY_INDEX(ent)];
	EntityIndex next_entity_p = next_entity;
	if (entity->mask != 0) {
		for (int i = 0; i < COMPONENT_COUNT; i++) {
			uint64_t mask = 1ULL<<i;
			if ((entity->mask & mask) == mask) {
				interfaces[i].
					rem_entity_immediately(ent);
			}
		}
	}

	next_entity = ENTITY_INDEX(entity->id);
	entity->id = CREATE_ENTITY(
		next_entity_p,
		ENTITY_GENERATION(entity->id));
	entity->enabled = false;
	entity->pending = false;
	entity->mask = 0;
}

void flush_entities(void)
{
	if (destroy_pending.count == 0) return;

	arr_foreach(Entity, ent_p, &destroy_pending) {
		Entity ent = *ent_p;
		destroy_entity_immediately(ent);

	}

	destroy_pending.count = 0;
}

uint64_t get_entity_component_mask(Entity ent)
{
	EntityDesc *entity;
	if (ENTITY_INDEX(ent) >= MAX_ENTITY_COUNT) return 0;
	entity = &entities[ENTITY_INDEX(ent)];
	if (entity->id != ent) return 0;

	return entity->mask;
}
