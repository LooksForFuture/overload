#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t Entity;
typedef uint32_t EntityIndex;
typedef uint16_t EntityGeneration;

#define MAX_ENTITY_INDEX UINT32_MAX
#define MAX_ENTITY_GENERATION UINT16_MAX

#define ENTITY_WORLD_MASK 0x000000000000FFFF
#define ENTITY_INDEX(ent) (EntityIndex)(((ent) >> 32) & 0xFFFFFFFF)
#define ENTITY_GENERATION(ent) (EntityGeneration)(((ent) >> 16) \
						  & 0x00000000FFFF)
#define ENTITY_WORLD(ent) ((ent) & RYU_ENTITY_WORLD_MASK)

void init_entities(void);

Entity create_entity(void);

bool is_entity_valid(Entity);

void destroy_entity(Entity);

/* caution: must only be used by the component declaration macros
   not for external use */
void component_added_to_entity(Entity, uint64_t);

/* caution: must only be used by the component declaration macros
   not for external use */
void component_removed_from_entity(Entity, uint64_t);

#endif
