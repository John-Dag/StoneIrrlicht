# StoneIrrlicht
Prototype of a 3D retro fps using the Irrlicht 3D engine. Features a level loader, command console, model loader, player movement, lighting, shaders, projectiles, and an event handler. 

Demo: The "demo" directory contains everything necessary to run the project (/demo/Release/Stone.exe). The latest build uses Irrlicht 14.0. Note: Use OpenGL 1.5, since the shader experimentation is specifically written for OpenGL. The demo build is not meant for use with DirectX.

Things to note:
The point lights attached to projectiles will not disappear after colliding. I was experementing with shaders, so you will notice a green "glow" that follows the projectiles. 

Controls:  
LMB: Fire weapon  
W: Forward  
S: Backward  
A: Strafe left  
D: Strafe right  
Spacebar: Jump  
F1: Open/close console  
ESC: Exit  

Console commands:  
Open/close the console with F1  
Use up/down arrow keys to scroll through commands  
"help": Show console help  
"load x": Load another map. Example: "load 13yard". Don't add the ".pk3" extension.    
"collision": Toggles player collision  
"gravity": Toggles player gravity  
