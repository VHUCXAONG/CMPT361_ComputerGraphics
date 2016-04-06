#### CMPT361 Assigment3

#####Task 1, a simple ray-tracer

* [8 marks] intersection test and local reflection
* [4 marks] shadow
* [4 marks] reflection
* [4 marks] chess board
* [4 marks] refraction
* [4 marks] stochastic ray generation and diffuse reflection
* [2 marks] spuersampling

###### Compiling instruction
  Simply type in <code>make</code> to compile the project.
  
  However, it is higher recomended to remove the depricated *.o file before runing makefile, therefore, please run <code>make clean</code> first.
###### how to run the code
  To run the code, simply execute <code>./raycast</code> programme, with the following arguments.
  
  * compulsory argments: 
  	
  	* selecting scene:
  		
  		<code>-d</code> default scene
  		
  		<code>-u</code> user defined scene
  	* maximum step of recursion:
  		<code> an interger </code> specifying the maximum number of recursion in ray tracing
  
  * optional arguments:
  
    * <code>+s</code> adding shadow to the scene
    * <code>+c</code> adding a chess board to the scene
    * <code>+l</code> adding reflection to the scene
    * <code>+r</code> adding refraction to the scene
    * <code>+f</code> adding diffuse reflection to the scene
    * <code>+p</code> adding anti-aliasing with super sampling to the scene

  * example

    * <code>./raycast -d 5 +s +l +f +p </code>
    * <code>./raycast -u 5 +s +c +r +l +f +p </code>

#####Bonus task
* [5 marks] read the simple mesh file and render two chess pieces with a chessboard.

###### Compiling instruction
  Simply type in <code>make</code> to compile the project.
  
  However, it is higher recomended to remove the depricated *.o file before runing makefile, therefore, please run <code>make clean</code> first.
###### how to run the code
  To run the code, simply execute <code>./raychess</code> programme, with the following arguments.
  
  * compulsory argments: 
  	
  	* maximum step of recursion:
  		<code> an interger </code> specifying the maximum number of recursion in ray tracing
  
  * optional arguments:
  
    * <code>+s</code> adding shadow to the scene
    * <code>+c</code> adding a chess board to the scene
    * <code>+l</code> adding reflection to the scene
    * <code>+r</code> adding refraction to the scene
    * <code>+f</code> adding diffuse reflection to the scene
    * <code>+p</code> adding anti-aliasing with super sampling to the scene

  * example

    * <code>./raychess 5 +s +l +f +p </code>
    * <code>./raychess 5 +s +c +r +l +f +p </code>
