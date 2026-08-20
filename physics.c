#include <physics.h>

#include <math.h>
#include <string.h>

#define MAX_BODY_COUNT 128

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
	bool pending_destroy;
	phBodyIndex index;
	phBodyIndex next_free;
	phBodyGeneration generation;
	phUserData data;
} BodySlot;

static phBodyIndex kinematic_count = 0;
static phBodyIndex ghost_count = 0;

static BodySlot slots[MAX_BODY_COUNT];
static phBodyIndex free_slot = 1;

static phBodyIndex destroy_queue[MAX_BODY_COUNT-1];
static int destroy_count = 0;

/*
  The physics system needs a record of the bodies in contact. Not only
  that, we also need to know which bodies were in contact in the previous
  update, and are not anymore, ... We can model this as a relation graph
  between bodies. The edge between two bodies represents contact, with
  at most only one edge being between each two bodies. Thus, we can turn
  it into a square matrix. Each entry of the matrix can be 1, or 0.
  Since we would have duplicates like (A, A), (A, B) and (B, A), we can
  keep an upper triangle of this matrix... I think you can guess the rest
  of this model.
  The contact set, is a bitset telling if two objects are in contact or
  not. To, be in contact, or not to be in contact. That is the question.

  Caution: The contacts are stored based on slot index.
 */
#define CONTACT_WORD_COUNT (((MAX_BODY_COUNT) + 63) / 64)
typedef struct {
	uint64_t bits[MAX_BODY_COUNT][CONTACT_WORD_COUNT];
} ContactSet;

static ContactSet contacts_a;
static ContactSet contacts_b;
/* contacts registered in the current update */
static ContactSet *current_contacts = &contacts_a;
/* contacts registered in the previous update */
static ContactSet *previous_contacts = &contacts_b;

#define MAX_EVENT_COUNT \
	(MAX_BODY_COUNT) * ((MAX_BODY_COUNT) - 1) / 2
static phCollisionEvent collision_events[MAX_EVENT_COUNT];
static int event_count = 0;

/* tells us if two bodies are in contact */
static bool get_contact(const ContactSet *set,
			phBodyIndex a, phBodyIndex b)
{
	if (a > b) {
		phBodyIndex tmp = a;
		a = b;
		b = tmp;
	}

	return (set->bits[a][b / 64] & (UINT64_C(1) << (b % 64))) != 0;
}

/* registers the contact of the bodies */
static void set_contact(ContactSet *set,
			phBodyIndex a, phBodyIndex b)
{
	if (a > b) {
		phBodyIndex tmp = a;
		a = b;
		b = tmp;
	}

	set->bits[a][b / 64] |= UINT64_C(1) << (b % 64);
}

/* registers the contact of the bodies */
static void clear_contact(ContactSet *set,
			phBodyIndex a, phBodyIndex b)
{
	if (a > b) {
		phBodyIndex tmp = a;
		a = b;
		b = tmp;
	}

	set->bits[a][b / 64] &= ~(UINT64_C(1) << (b % 64));
}

static void clear_slot_contacts(ContactSet *set, phBodyIndex body)
{
	for (phBodyIndex i = 1; i < MAX_BODY_COUNT; i++) {
		if (i == body) continue;

		clear_contact(set, body, i);
	}
}

void ph_init(void)
{
	kinematic_count = 0;
	ghost_count = 0;
	free_slot = 1;
	destroy_count = 0;
	event_count = 0;

	memset(current_contacts, 0, sizeof(ContactSet));
	memset(previous_contacts, 0, sizeof(ContactSet));

	current_contacts = &contacts_a;
	previous_contacts = &contacts_b;

	for (int i = 0; i < MAX_BODY_COUNT - 1; i++) {
		slots[i] = (BodySlot){true, false, 0, i+1, 0, 0};
	}
	slots[MAX_BODY_COUNT-1] = (BodySlot){true, false, 0, 0, 0, 0};
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
		slot->pending_destroy = false;
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
		slot->pending_destroy = false;
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

static void destroy_body_immediate(phBodyIndex slot_index)
{
	phBodyIndex index, last_ghost, last_kin;
	BodySlot *slot;
	enum phBodyType type;

	slot = &slots[slot_index];
	index = slot->index;
	type = bodies.type[index];
	last_ghost = kinematic_count + ghost_count;
	last_kin = kinematic_count;
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

	clear_slot_contacts(current_contacts, slot_index);
	clear_slot_contacts(previous_contacts, slot_index);

	slot->empty = true;
	slot->pending_destroy = false;
	slot->index = 0;
	slot->next_free = free_slot;
	slot->data = 0;

	free_slot = slot_index;
}

void ph_destroy_body(phBody body) {
	BodySlot *slot;
	if (!ph_is_body_valid(body)) return;

	slot = &slots[PH_INDEX(body)];
	if (slot->pending_destroy) return;

	slot->pending_destroy = true;
	destroy_queue[destroy_count++] = PH_INDEX(body);
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

static void queue_event(phBodyIndex a, phBodyIndex b,
			Vec2 normal, float pen, enum phEventType type)
{
	if (event_count >= MAX_EVENT_COUNT) return;

	if (a > b) {
		phBodyIndex tmp = a;
		a = b;
		b = tmp;

		normal.x = -normal.x;
		normal.y = -normal.y;
	}

	collision_events[event_count++] = (phCollisionEvent){
		.body_a = CREATE_BODY(a, slots[a].generation),
		.body_b = CREATE_BODY(b, slots[b].generation),
		.normal = normal,
		.penetration = pen,
		.type = type
	};
}

static void register_contact(int dense_a, int dense_b,
			     Vec2 normal, float pen)
{
	phBodyIndex a = bodies.rev_map[dense_a];
	phBodyIndex b = bodies.rev_map[dense_b];
	if (get_contact(current_contacts, a, b)) return;

	set_contact(current_contacts, a, b);
	enum phEventType type =
		get_contact(previous_contacts, a, b) ?
		PH_COLLISION_STAY : PH_COLLISION_ENTER;
	queue_event(a, b, normal, pen, type);
}

static bool detect_collision(int a, int b,
			     Vec2 *normal, float *penetration)
{
	float dx = bodies.x[b] - bodies.x[a];
	float dy = bodies.y[b] - bodies.y[a];

	float dist_sq = dx*dx + dy*dy;
	float min_dist = bodies.radius[a] + bodies.radius[b];
	float min_dist_sq = min_dist * min_dist;

	if (dist_sq >= min_dist_sq) return false;

	if (dist_sq <= 0.00000001f) {
		*normal = (Vec2){1.0f, 1.0f};
		*penetration = min_dist;
		return true;
	}

	float dist = sqrtf(dist_sq);
	*normal = (Vec2){dx / dist, dy / dist};
	*penetration = min_dist - dist;

	return true;
}

static bool resolve_kinematics(int a, int b)
{
	Vec2 normal;
	float penetration;

	if (!detect_collision(a, b, &normal, &penetration)) return false;

	register_contact(a, b, normal, penetration);

	/* position correction */
	bodies.x[a] -= normal.x * penetration * 0.5f;
	bodies.y[a] -= normal.y * penetration * 0.5f;
	bodies.x[b] += normal.x * penetration * 0.5f;
	bodies.y[b] += normal.y * penetration * 0.5f;

	return true;
}

static void detect_all_contacts(void)
{
	int count = kinematic_count + ghost_count;

	for (int i = 1; i <= count; i++) {
		if (!bodies.enabled[i]) continue;

		for (int j = i + 1; j <= count; j++) {
			if (!bodies.enabled[j])
				continue;

			Vec2 normal;
			float penetration;
			if (!detect_collision(i, j, &normal,
					      &penetration)) continue;
			register_contact(i, j, normal, penetration);
		}
	}
}

static void detect_exits(void)
{
	for (phBodyIndex a = 1; a < MAX_BODY_COUNT; a++) {
		for (phBodyIndex b = a + 1; b < MAX_BODY_COUNT; b++) {
			if (!get_contact(previous_contacts, a, b))
				continue;

			if (get_contact(current_contacts, a, b))
				continue;

			/* While bodies don't get destroyed during an
			 update, we keep this check*/
			if (slots[a].empty || slots[b].empty) continue;

			queue_event(a, b, (Vec2){0}, 0.0f,
				    PH_COLLISION_EXIT);
		}
	}
}

void ph_update(float dt, int iter_count)
{
	memset(current_contacts, 0, sizeof(ContactSet));
	event_count = 0;

	for (int i = 0; i < destroy_count; i++) {
		destroy_body_immediate(destroy_queue[i]);
	}
	destroy_count = 0;

	for (int i = 1; i <= kinematic_count + ghost_count; i++) {
		if (!bodies.enabled[i]) continue;

		bodies.x[i] += bodies.vx[i] * dt;
		bodies.y[i] += bodies.vy[i] * dt;
	}

	detect_all_contacts();
	for (int iter = 0; iter < iter_count; iter++) {
		for (int i = 1; i <= kinematic_count; i++) {
			if (!bodies.enabled[i]) continue;
			for (int j = i + 1; j <= kinematic_count; j++) {
				if (!bodies.enabled[j]) continue;
				resolve_kinematics(i, j);
			}
		}
	}

	/*
	  Positional correction may have pushed a body into another, so
	  we must catch these contacts created by the solver.
	 */
	detect_all_contacts();

	/*
	  any contact present in the last update, but absent from this
	  one has exited.
	*/
	detect_exits();

	ContactSet *tmp = previous_contacts;
	previous_contacts = current_contacts;
	current_contacts = tmp;
}

const phCollisionEvent *ph_get_events(int *count)
{
	*count = event_count;
	return &collision_events[0];
}
