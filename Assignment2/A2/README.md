###CMPT361 AssignmentII
#### Bowei Yuan 301295751
--
###File Directory
1. **FruitTetris.cpp** the main source code file, the code skeleton was provided by the instructor, additions and modifications are made to complete the game.
2. **fshader.glsl** fragment shader, provided by the instructor and TA.
3. **vshader.glsl** vertext shader, provided by the instructor and TA.
4. **include/** this directory contains related header files and shader initialization program, provided by the instructor and TA.
5. **Makefile** this is the makefile which helps to compile and link the dependencies with a simple <code>make</code> command.

###Compiling instruction

  The compiling process is straightforward, simply run <code>make</code> command under the source directory, which contains the *Makefile* file.

  **NOTE:** To guarantee the consistency and integrity, you are encouraged to run ***make clean*** to clean all the compiled dependencies first.
  
  
###Game Instruction
1. **Viewing**:
  
  <code> o</code> <code>CTRL+LEFT</code> move the camera to the left
  
  <code> O </code> <code>CTRL + RIGHT </code> move the camera to the right
  
  <code>x</code>: expand the viewing port horizontally.
  
  <code>X</code>: shrink the viewing port horizontally.
  
  <code>y</code>: expand the viewing port vertically.
  
  <code>Y</code>: shrink the viewing port vertically.

2. **Robotic Arms**:

	<code>w</code>: move the robotic arm higher
	
	<code>s</code>: move the robotic arm lower
	
	<code>a</code>: move the robotic arm left
	
	<code>d</code>: move the robotic arm right
	
	<code>SPACE</code>: drop the tile on the arm

3. **Tile**:

	<code>up</code>: rotate the tile
	
	<code>down</code>: when the tile is not held by the arm, accelerate it falling down the the bottom
		
4. **Game Logic**:
	
	<code>ESC</code> <code>q</code>: quit the game
 	
	<code>r</code>: restart the game
 