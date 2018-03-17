Artificial Intelligence - Project 2

Author: Euclides Araujo

User input keys:
UP, DOWN, LEFT and RIGHT arrows: Control the Player (Forward, Backward and Turn)
A, D: Control the Circle Radius ( Used in Wander Behaviour )
W, S: Control the Circle Distance from the Game Object( Used in Wander Behaviour )
Q, E: Control the Angle change ( Used in Wander Behaviour )
ENTER: Enter the debug mode
ESC: Quit the application

Configuration of the scene is done via the objects.txt file.
Each line can create 1 object in the scene, the line structure is:
meshname posX posY posZ scale mass

Meshname is String pointing to the corresponding mesh in the meshlist.txt
The other values are float

meshname numberOfObjects posX posY posZ scale isRandom? rangeX rangeY rangeZ rangeScale

example:

example:
terrain         1  0.0  0.0  0.0  1.0 false 0.0 0.0 0.0 0.0
meeseeks	1  6.0  0.0  6.0  1.0 false 0.0 0.0 0.0 0.0
scary		1  4.0  0.0 -3.0  1.0 false 0.0 0.0 0.0 0.0
morty		1 -3.0  0.0  3.0  1.0 false 0.0 0.0 0.0 0.0