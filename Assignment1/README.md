###CMPT361 Assignment01 FruitTetris
####Bowei Yuan, 301295751
--
##File Directory
1. **FruitTetris.cpp** the main source code file, the code skeleton was provided by the instructor, additions and modifications are made to complete the game.
2. **fshader.glsl** fragment shader, provided by the instructor and TA.
3. **vshader.glsl** vertext shader, provided by the instructor and TA.
4. **include/** this directory contains related header files and shader initialization program, provided by the instructor and TA.
5. **Makefile** this is the makefile which helps to compile and link the dependencies with a simple <code>make</code> command.

##Compiling instruction
The compiling process is straightforward, simply run <code>make</code> command under the source directory, which contains the *Makefile* file.

**NOTE:** To guarantee the consistency and integrity, you are encouraged to run ***make clean*** to clean all the compiled dependencies first.

##Feature Checklist and Game Instructions
1. **Tile and Grid Rendering and Tile Falling**
  
  The tile and grid initialization routines are already implemented in the code skeleton. While the following steps are still needed:
	1. As for the current falling tile, six different shapes (L, reverse L, S, reverse S, T, I) as well as five different colours (Purple, Red, Green, Red, Orange) are defined and are randomly assigned to each blocks in the current tile when a new tile is generated.
	
  2. As for the Board, when one falling tile is settled, it has to be updated, therefore a new routine
	<code> void updateBoard() </code>
    is added to update the board each time the blocks on the board change.
     
  3. To make the Tile falling, one timer is added

	~~~cpp
		glutTimerFunc(300, timer, 10); //register a timer in main
	
		void timer(int value) {
			movetile(vec2(0, -1));//move the current tile down
			updatetile(); // update the current tile
			glutTimerFunc(300, timer, 10);//register a new function
		}
	~~~

2. **Stack Up**

	Each time before we move/rotate the tile, we firstly judge whether it after this move/rotate it would collide with settled tiles, if any collision occurs, the function <code> settle() </code> woudl be called, which moves the colour and position information from current tile to the board in accordance with the positions it occupies, then a new tile would be generated.

3. **Key Stroke Interaction and Tile Movements**
  
  You can interact with the game by pressing following keys on your keyboard:

	1. Direction Keys:
		1. <code>up</code>: rotate the current tile for 90 degree counterclockwise.
		2. <code>down</code>: accelerate the falling currenttile
		3. <code>left</code> <code>right</code>: move the current tile to the specified direction by 1 block.
	2. Exit Game:
	
	  you can exit the game by pressing either <code>ESC</code> or <code>q</code>
	  
	3. Restart Game:
	
	  You could restart the game by pressing <code>r</code>.  

  The tile movements and rotations are implemented with following functions:
 	
 	1. <code>bool movetile(vec2 direction);</code> which moves the current tile towards the direction specified by the vector, the movements are executed after collision checks and crossing boundary checks, and then a new tile would be generated.
 	2. <code>void rotate(int direction)</code>, the direction is either 1 or -1, which respectively denotes counterclockwise rotation or clockwise rotation, the rotations would be executed after collision checks and cross boundary checks.
 
4. **Additional Game Logic**
  1. Checking fullrow is done by the function <code>void checkfullrow(int row)</code>. This functions is executed each time when a new tile is settled, which checks all the rows involved due to this settlement, and label all the rows that are full.
  2. Checking consecutive same coloured patterns is done by the function <code> bool checkcolour(int x, int y) </code>, this functions checks all the possible patterns around coordinate (x,y), and then mark the coordinates which should be removed. One thing to note is that new consecutive patterns may be caused by such removals, and therefore we should check all the involved blocks after the removal till there are no such patterns exist.

5. **Additional features**
  1. **Pause Game**: Press <code>p</code> the game would be stopped. The original skeleton did not give a function to pause the game, but we can modify the callback functions, which are called in the main loop. When we press <code>p</code>, one state variable is changed (negated). If it is in pause state, all call back functions return immediately without processing anything, and the Game is therefore paused, until <code>p</code> is pressed again.