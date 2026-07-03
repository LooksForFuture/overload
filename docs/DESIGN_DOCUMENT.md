# Design document

This is a 2D top down shooter game, with some simplistic sci-fi and
cartoonish visual style. Right now, there is no story.

The player character is at the center of the screen, and can move freely
in any direction. The player is always looking towards the mouse.
The enemies swarm the player from all around the screen. There is no
obstacle between the player and the enemies. There are no bounds
to the world. If the player gets too distant from the origin (0, 0)
all the world would be shifted towards the origin without the player
noticing it. There would be multiple weapons, and multiple enemy types.
Enemies drop ammo and powerups when destroyed.
Some players abilities can be used after being charged.
The abilities are either charged after a specific time, or by destroying
a specific count of enemies.