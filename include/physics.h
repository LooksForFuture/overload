#ifndef PHYSICS_H
#define PHYSICS_H

/* The physics subsystem. Uses ph prefix.
   It must not rely on other subsystems and must be able to work in
   isolation and be portable across projects.

   Everything is a circle here. All our simulations are done based on
   circles. There are two types of colliders here. One is kinematic which
   means that they can be moved only by themselves and the force of other
   colliders has no impact on them. The other type is the ghost type
   which is just for collision check and work as triggers.
   They move thorugh everything. Like ghosts. creepy.

   The coordinate system is 2D (x, y) with (0, 0) being the origin. The x
   component is from left to right, and y is from down to up.
   The rotation is done counter-clockwise based on radians.

   Since passing pointers around is highly dangerous, we pass integer
   identifiers. So, an id which equals to zero, means error.
 */

#include <gmath.h>

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t phBodyIndex;
typedef uint8_t phBodyGeneration;
typedef uint16_t phBody;
typedef uint64_t phUserData;

#define PH_INDEX(bd) (phBodyIndex)(((bd) >> 8) & 0xFF)
#define PH_GENERATION(bd) (phBodyGeneration)((bd) & 0xFF)

enum phBodyType {PH_BODY_NONE, PH_KINEMATIC, PH_GHOST};

enum phEventType {
	PH_COLLISION_ENTER,
	PH_COLLISION_STAY,
	PH_COLLISION_EXIT
};

typedef struct {
	phBody body_a;
	phBody body_b;
	Vec2 normal;
	float penetration;
	enum phEventType type;
} phCollisionEvent;

/* init the physics system */
void ph_init(void);

/* free all the resources used */
void ph_shutdown(void);

/*
  All the properties would be set to zero upon creation.
  The body would be enabled, but since the radius is zero, would not
  participate in simulation.
 */
phBody ph_new_body(enum phBodyType);

bool ph_is_body_valid(phBody);

void ph_destroy_body(phBody);

bool ph_is_body_enabled(phBody);

void ph_set_body_enabled(phBody, bool);

Vec2 ph_get_body_position(phBody);

void ph_set_body_position(phBody, Vec2);

float ph_get_body_radius(phBody);

void ph_set_body_radius(phBody, float);

Vec2 ph_get_body_velocity(phBody);

void ph_set_body_velocity(phBody, Vec2);

enum phBodyType ph_get_body_type(phBody);

/* TODO: implement body type change */

phUserData ph_get_body_user_data(phBody); /* is zero by default */

void ph_set_body_user_data(phBody, phUserData);

void ph_update(float, int);

const phCollisionEvent *ph_get_events(int *);

#endif /* PHYSICS_H */
