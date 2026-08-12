#include <physics.h>

#include <math.h>

#define MAX_BODY_COUNT 128
#define ITER_COUNT 5

#define CREATE_BODY(i, g) (((uint16_t)(i) << 8)|((uint8_t)(g)))

/* All physics bodies(colliders) are all stored in a dense array. Since
   there are two types of colliders(kinematic and ghost), we partition
   the dense array. The kinematic bodies are stored first, and the ghosts
   second. The partitions are dynamic in size and can grow or shrink.

   The algorithm:
   Upon creation of a ghost body, we just put it at the end of the array.
   [K, K, K, G, G, G] -> [K, K, K, G, G, G, new G]

   And for the kinematic bodies, we move the first ghost collider to the
   end of the array, and put the new kinematic collider in there.
   [K, K, first G, G, G] -> [K, K, new K, G, G, first G]

   Upon destruction of a ghost body, we just swap it with the last ghost.

   Upon destruction of a kinematic, we swap the last kinematic with it,
   and put the last ghost in place of the last kinematic.
   [K, to die K, K, last K, G, G, last G] -> [K, last K, K, last G, G, G]

   And since this approach makes the bodies' indices unstable, we need a
   parallel sparse array of the same size which maps keys to bodies. */

typedef struct {
	bool enabled;
	float x, y;
	float radius;
	float vx, vy; /* velocity */
	enum phBodyType type;
	phBodyIndex rev_map;
} Body;

struct {
	bool enabled[MAX_BODY_COUNT];
	float x[MAX_BODY_COUNT];
	float y[MAX_BODY_COUNT];
	float radius[MAX_BODY_COUNT];
	float vx[MAX_BODY_COUNT];
	float vy[MAX_BODY_COUNT];
	enum phBodyType type[MAX_BODY_COUNT];
	phBodyIndex rev_map[MAX_BODY_COUNT];
} bodies;

typedef struct {
	bool empty;
	phBodyIndex index;
	phBodyIndex next_free;
	phBodyGeneration generation;
	phUserData data;
} BodySlot;

static phBodyIndex kinematic_count = 0;
static phBodyIndex ghost_count = 0;

static BodySlot slots[MAX_BODY_COUNT];
static phBodyIndex free_slot = 1;

void ph_init(void)
{
	for (int i = 0; i < MAX_BODY_COUNT - 1; i++) {
		slots[i] = (BodySlot){true, 0, i+1, 0, 0};
	}
	slots[MAX_BODY_COUNT-1] = (BodySlot){true, 0, 0, 0, 0};
}

void ph_shutdown(void)
{}

/* Cauton: Doesn't bound check */
static void set_body(phBodyIndex index, Body *bd)
{
	bodies.enabled[index] = bd->enabled;
	bodies.x[index] = bd->x;
	bodies.y[index] = bd->y;
	bodies.radius[index] = bd->radius;
	bodies.vx[index] = bd->vx;
	bodies.vy[index] = bd->vy;
	bodies.type[index] = bd->type;
	bodies.rev_map[index] = bd->rev_map;
}

/* Cauton: Doesn't bound check */
/* static Body get_body(phBodyIndex index)
{
	Body bd;
	bd.enabled = bodies.enabled[index];
	bd.x = bodies.x[index];
	bd.y = bodies.y[index];
	bd.radius = bodies.radius[index];
	bd.vx = bodies.vx[index];
	bd.vy = bodies.vy[index];
	bd.type = bodies.type[index];
	bd.rev_map = bodies.rev_map[index];
	return bd;
	} */

/*
  moves the body at index2 to index 1
  Cauton: Doesn't bound check
*/
static void mov_body(phBodyIndex index1, phBodyIndex index2)
{
	bodies.enabled[index1] = bodies.enabled[index2];
	bodies.x[index1] = bodies.x[index2];
	bodies.y[index1] = bodies.y[index2];
	bodies.radius[index1] = bodies.radius[index2];
	bodies.vx[index1] = bodies.vx[index2];
	bodies.vy[index1] = bodies.vy[index2];
	bodies.type[index1] = bodies.type[index2];
	bodies.rev_map[index1] = bodies.rev_map[index2];
}

phBody ph_new_body(enum phBodyType type)
{
	phBody id;
	phBodyIndex last_index = kinematic_count + ghost_count + 1;
	BodySlot *slot = &slots[free_slot];
	Body bd = (Body){
		true,
		0, 0,
		0,
		0, 0,
		type,
		free_slot
	};

	if (free_slot == 0) return 0;
	if (type == PH_GHOST) {
		set_body(last_index, &bd);
		slot->empty = false;
		slot->index = last_index;
		slot->generation++;
		id = CREATE_BODY(free_slot, slot->generation);
		free_slot = slot->next_free;
		ghost_count++;
		return id;
	} else if (type == PH_KINEMATIC) {
		/* send first ghost to the last index */
		if (ghost_count > 0) {
			phBodyIndex last_ghost = kinematic_count + 1;
			phBodyIndex rev = bodies.rev_map[last_ghost];
			mov_body(last_index, last_ghost);
			slots[rev].index = last_index;
		}
		set_body(kinematic_count + 1, &bd);
		slot->empty = false;
		slot->index = kinematic_count + 1;
		slot->generation++;
		id = CREATE_BODY(free_slot, slot->generation);
		free_slot = slot->next_free;
		kinematic_count++;
		return id;
	}

	return 0;
}

bool ph_is_body_valid(phBody body)
{
	phBodyIndex index = PH_INDEX(body);
	phBodyGeneration generation = PH_GENERATION(body);

	if (index >= MAX_BODY_COUNT ||
	    slots[index].generation != generation ||
	    slots[index].empty) {
		return false;
	}

	return true;
}

void ph_destroy_body(phBody body)
{
	phBodyIndex index, last_ghost, last_kin;
	enum phBodyType type;
	if (!ph_is_body_valid(body)) return;

	index = slots[PH_INDEX(body)].index;
	last_ghost = kinematic_count + ghost_count + 1;
	last_kin = kinematic_count + 1;
	type = bodies.type[index];
	if (type == PH_GHOST) {
		if (index < last_ghost) {
			phBodyIndex rev = bodies.rev_map[last_ghost];
			slots[rev].index = index;
			mov_body(index, last_ghost);
		}
		ghost_count--;
	} else if (type == PH_KINEMATIC) {
		if (index < last_kin) {
			phBodyIndex rev = bodies.rev_map[last_kin];
			slots[rev].index = index;
			mov_body(index, last_kin);
		}

		if (ghost_count > 0) {
			phBodyIndex rev = bodies.rev_map[last_ghost];
			slots[rev].index = last_kin;
			mov_body(last_kin, last_ghost);
		}
		kinematic_count--;
	}
}

bool ph_is_body_enabled(phBody body)
{
	if (ph_is_body_valid(body)) {
		return bodies.enabled[slots[PH_INDEX(body)].index];
	}

	return false;
}

void ph_set_body_enabled(phBody body, bool state)
{
	if (ph_is_body_valid(body)) {
		bodies.enabled[slots[PH_INDEX(body)].index] = state;
	}
}

Vec2 ph_get_body_position(phBody body)
{
	if (ph_is_body_valid(body)) {
		phBodyIndex index = slots[PH_INDEX(body)].index;
		return (Vec2){bodies.x[index], bodies.y[index]};
	}

	return (Vec2){0};
}

void ph_set_body_position(phBody body, Vec2 new_pos)
{
	if (ph_is_body_valid(body)) {
		phBodyIndex index = slots[PH_INDEX(body)].index;
		bodies.x[index] = new_pos.x;
		bodies.y[index] = new_pos.y;
	}
}

float ph_get_body_radius(phBody body)
{
	if (ph_is_body_valid(body)) {
		phBodyIndex index = slots[PH_INDEX(body)].index;
		return bodies.radius[index];
	}

	return 0;
}

void ph_set_body_radius(phBody body, float rad)
{
	if (ph_is_body_valid(body)) {
		phBodyIndex index = slots[PH_INDEX(body)].index;
		bodies.radius[index] = rad;
	}
}

Vec2 ph_get_body_velocity(phBody body)
{
	if (ph_is_body_valid(body)) {
		phBodyIndex index = slots[PH_INDEX(body)].index;
		return (Vec2){bodies.vx[index], bodies.vy[index]};
	}

	return (Vec2){0};
}

void ph_set_body_velocity(phBody body, Vec2 vel)
{
	if (ph_is_body_valid(body)) {
		phBodyIndex index = slots[PH_INDEX(body)].index;
		bodies.vx[index] = vel.x;
		bodies.vy[index] = vel.y;
	}
}

enum phBodyType ph_get_body_type(phBody body)
{
	if (ph_is_body_valid(body)) {
		phBodyIndex index = slots[PH_INDEX(body)].index;
		return bodies.type[index];
	}

	return PH_BODY_NONE;
}

/* TODO: implement body type change */

phUserData ph_get_body_user_data(phBody body)
{
	if (ph_is_body_valid(body)) {
		return slots[PH_INDEX(body)].data;
	}
	return 0;
}

void ph_set_body_user_data(phBody body, phUserData data)
{
	if (ph_is_body_valid(body)) {
		slots[PH_INDEX(body)].data = data;
	}
}

static void resolve_kinematics(int a, int b)
{
	float dx = bodies.x[b] - bodies.x[a];
	float dy = bodies.y[b] - bodies.y[a];
	float dist = sqrtf(dx*dx + dy*dy);
	float min_dist = bodies.radius[a] + bodies.radius[b];
	float overlap = min_dist - dist;
	float nx, ny;
	
	if (dist >= min_dist || dist <= 0.0001f) return;

	/* normalized direction from a to b */
	nx = dx / dist;
	ny = dy / dist;

	/* position correction */
	bodies.x[a] -= nx * overlap * 0.5f;
	bodies.y[a] -= ny * overlap * 0.5f;
	bodies.x[b] += nx * overlap * 0.5f;
	bodies.y[b] += ny * overlap * 0.5f;

	return;
}

void ph_update(float dt)
{
	for (int i = 1; i <= kinematic_count; i++) {
		if (!bodies.enabled[i]) continue;

		bodies.x[i] += bodies.vx[i] * dt;
		bodies.y[i] += bodies.vy[i] * dt;
	}

	for (int iter = 0; iter < ITER_COUNT; iter++) {
		for (int i = 1; i <= kinematic_count; i++) {
			for (int j = i + 1; j <= kinematic_count; j++) {
				if (!bodies.enabled[j]) continue;
				resolve_kinematics(i, j);
			}
		}
	}
}
