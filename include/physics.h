#ifndef PHYSICS_H
#define PHYSICS_H

/* The physics subsystem. Uses ph prefix.
   Everything is a circle here. All our simulations are done based on
   circles. There are two types of colliders here. One is kinematic which
   means that they can be moved only by themselves and the force of other
   colliders has no impact on them. The other type is the trigger type
   which is just for collision check. They move thorugh everything.
   Like ghosts. creepy.

   It must not rely on other subsystems and must be able to work in
   isolation and be portable across projects.

   The coordinate system is 2D (x, y) with (0, 0) being the origin. They x
   component is from lef to right, and y is from down to up.
   The rotation is done counter-clockwise based on radians.
 */

#include <stdint.h>

typedef uint16_t phBody;

enum phBodyType {PH_BODY_KINEMATIC, PH_BODY_TRIGGER};

/* init the physics system */
void ph_init(void);

/* free all the resources used */
void ph_shutdown(void);

phBody ph_new_body(enum phBodyType);

#endif /* PHYSICS_H */
