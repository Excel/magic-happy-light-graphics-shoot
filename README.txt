________                                         _________.__                   __        ___________.__             ________                                     .__                
\______ \____________     ____   ____   ____    /   _____/|  |__   ____   _____/  |_  /\  \__    ___/|  |__   ____   \______ \____________     ____   ____   ____ |__| ____    ____  
 |    |  \_  __ \__  \   / ___\ /  _ \ /    \   \_____  \ |  |  \ /  _ \ /  _ \   __\ \/    |    |   |  |  \_/ __ \   |    |  \_  __ \__  \   / ___\ /  _ \ /    \|  |/    \  / ___\ 
 |    `   \  | \// __ \_/ /_/  >  <_> )   |  \  /        \|   Y  (  <_> |  <_> )  |   /\    |    |   |   Y  \  ___/   |    `   \  | \// __ \_/ /_/  >  <_> )   |  \  |   |  \/ /_/  >
/_______  /__|  (____  /\___  / \____/|___|  / /_______  /|___|  /\____/ \____/|__|   \/    |____|   |___|  /\___  > /_______  /__|  (____  /\___  / \____/|___|  /__|___|  /\___  / 
        \/           \//_____/             \/          \/      \/                                         \/     \/          \/           \//_____/             \/        \//_____/  

Chris Piette (cmpiette) and John Tran (jqtran) :D

Technical Features:

Bounding Box/Collision
Acquire the vertex data for a model. Iterate through the vertices and keep track of the max/min corners of the box. After iterating through all vertex data, the min vector will have the least x/y/z coordinates and the max vector will have the greatest x/y/z coordinates. To detect collision, the minimum and maximum coordinates of two objects are compared to check for overlap. 

Piecewise Bezier Curves
For each moving object, a closed path is generated that is comprised of Bezier curves. Generally, when adding points, if the next point added would complete the Bezier curve, instead, the midpoint between the previous point and the next point is added to be the control point. The next point is added as a smoothing point instead. Before completing the circuit, the program checks that there is collinearity between the origin point and the points before/after.

Shaders
The "psycho" shader is a modification of the pulse shader in which time is used as the primary variable when computing perturbations of the model and its shifts in color. 
The shaders applied to enemy/friendly units are modifications of the reflect/refract shaders, with their colors modified to reflect alignment with the player.

Terrain Generation/Texture Mapping
We re-implemented the terrain generation in Lab 4, and modified the bounds and perturbations of the functions to easily blend in with the skybox. 

Skybox
We re-implemented the skybox construction in Lab 9 and created a custom-made skybox. 

Overall:
This is a first person rail shooter. Enemy/friendly units are generated along the Bezier curve, with respect to your current position. Clicking a mouse button will shoot a dragon projectile towards the center of the screen. Hitting red enemy units will grant 1 point. Hitting green friendly units will subtract 5 points. There is a maximum of 20 units on the field. To prevent the field from being oversaturated with friendly units, every unit has a set lifespan. Bullets also have a set lifespan, but have a far-reaching range. Bounding boxes can be viewed by pressing C. If you do manage to go through the entire Bezier curve path (which will be rare because the path is relatively long), a new path is generated that still maintains collineraity at the origin. 

Bugs/Memory Leaks:
To load models, we had to use absolute paths, or else fopen would throw null pointers at us. To run this on other computers, in /sources/lab/glwidget.cpp, in createModels(), change the absolute paths to the appropriate paths for each model. Sorry!

We believe there are no memory leaks at the moment! :D

Other Notes:
For optional viewing on a monitor (not projector):

In /sources/lab/glwidget.cpp, line 168, change the arguments for ambientColor[] to .55f, .3f, .55f, 1.0f. 

Thank you!
