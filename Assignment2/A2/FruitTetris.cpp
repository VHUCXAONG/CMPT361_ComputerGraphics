/* CMPT 361 Assignment 1 - FruitTetris implementation Sample Skeleton Code - This is ONLY a skeleton code showing: How to use multiple buffers to store different objects An efficient scheme to represent the grids and blocks - Compile and Run: Type make in terminal, then type ./FruitTetris This code is extracted from Connor MacLeod's (crmacleo@sfu.ca) assignment submission by Rui Ma (ruim@sfu.ca) on 2014-03-04.  Modified in Sep 2014 by Honghua Li (honghual@sfu.ca).
*/

#include "include/Angel.h"
#include <cstdlib>
#include <iostream>

using namespace std;


// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 495;
int ysize = 660;
int r_type; //random number for current tile type
int r_colour; //random number for current block colour
int state; //current tile state
unsigned linecounter = 0; //counting the number of full lines
unsigned armcount = 0;
// current tile
vec2 tile[4]; // An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec4 tilecolour[4];
vec2 tilepos = vec2(5, 19); // The position of the current tile using grid coordinates ((0,0) is the bottom left corner)
bool hold=true;
bool removetable[10][20]; //tiles to remove each time
// An array storing all possible orientations of all possible tiles
// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
//L, mirror L, S, mirror S, T, I
GLuint model_view;
GLuint projection;

GLfloat radius = 10;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

GLfloat  b_left = -9.5, b_right = 9.5; 
GLfloat  b_bottom = -12, b_top = 12;
GLfloat  zNear = 0.0, zFar = 50.0;
const GLfloat  dr = 5.0 * DegreesToRadians;
vec2 allRotationsShapes[6][4][4] =
{
	{
			{vec2(0, 0), vec2(-1,0), vec2(1, 0), vec2(-1,-1)},
			{vec2(0,0), vec2(0, -1), vec2(0, 1), vec2(1, -1)},
			{vec2(0, 0), vec2(1,0), vec2(-1, 0), vec2(1,  1)},
			{vec2(0, 0), vec2(0, 1), vec2(0,-1), vec2(-1, 1)}

	},
	{
			{vec2(0,0),vec2(0,1),vec2(0,-1),vec2(-1,-1)},
			{vec2(0,0),vec2(-1,0),vec2(1,0),vec2(1,-1)},
			{vec2(0,0),vec2(0,-1),vec2(0,1),vec2(1,1)},
			{vec2(0,0),vec2(1,0),vec2(-1,0),vec2(-1,1)}
	},

	{
			{vec2(0, 0), vec2(-1,0), vec2(0, -1), vec2(1,-1)},
			{vec2(0, 0), vec2(0, -1), vec2(1,0), vec2(1, 1)},
			{vec2(0, 0), vec2(-1,0), vec2(0, -1), vec2(1,-1)},
			{vec2(0, 0), vec2(0, -1), vec2(1,0), vec2(1, 1)}
	},
	{
			{vec2(0,0),vec2(1,0),vec2(0,-1),vec2(-1,-1)},
			{vec2(0,0),vec2(0,1),vec2(1,0),vec2(1,-1)},
			{vec2(0,0),vec2(1,0),vec2(0,-1),vec2(-1,-1)},
			{vec2(0,0),vec2(0,1),vec2(1,0),vec2(1,-1)}
	},

	{
			{vec2(0, 0), vec2(-1,0), vec2(1, 0), vec2(0, -1)},
			{vec2(0,0), vec2(0, -1), vec2(0, 1), vec2(1, 0)},
			{vec2(0, 0), vec2(1,0), vec2(-1, 0), vec2(0,  1)},
			{vec2(0, 0), vec2(0, 1), vec2(0,-1), vec2(-1, 0)},


	},

	{
			{vec2(-2, 0), vec2(-1,0), vec2(0, 0), vec2(1,0)},
			{vec2(0, -2), vec2(0, -1), vec2(0,0), vec2(0, 1)},
			{vec2(-2, 0), vec2(-1,0), vec2(0, 0), vec2(1,0)},
			{vec2(0, -2), vec2(0, -1), vec2(0,0), vec2(0, 1)},
	}
};

// colors
vec4 orange = vec4(1.0, 0.5, 0.0, 1.0); 
vec4 white  = vec4(1.0, 1.0, 1.0, 1.0);
vec4 black  = vec4(0.0, 0.0, 0.0, 1.0); 
vec4   red  = vec4(1.0, 0.0, 0.0, 1.0);
vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
vec4 green 	= vec4(0.0, 1.0, 0.0, 1.0);
vec4   blue = vec4(0.0, 0.0, 1.0, 1.0);
vec4 purple = vec4(1.0,0.0,1.0,1.0);
vec4 grey = vec4(0.33, 0.33, 0.33,1.0);
//board[x][y] represents whether the cell (x,y) is occupied
bool board[10][20]; 

//An array containing the colour of each of the 10*20*2*3 vertices that make up the board
//Initially, all will be set to black. As tiles are placed, sets of 6 vertices (2 triangles; 1 square)
//will be set to the appropriate colour in this array before updating the corresponding VBO
vec4 boardcolours[7200];

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;

// locations of uniform variables in shader program
GLuint locxsize;
GLuint locysize;

// VAO and VBO
GLuint vaoIDs[6]; // One VAO for each object: the grid, the board, the current piece
GLuint vboIDs[12]; // Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)

//-------------------------------------------------------------------------------------------------------------------
//When the board is to be updated, update the VBO containing the board's vertext color data
void updateBoard()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]); // Bind the VBO containing current board vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(boardcolours), boardcolours); // Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
//------------------------------------------------------------------------------------------------------------------
//add two triangles of a facet into the buffer
void addvertex(vec4 *board,vec4 *polygon,int facet, int x, int y, int a, int b, int c, int d)
{
	board[36*(10*y+x)+facet*6 + 0] = polygon[a];
	board[36*(10*y+x)+facet*6 + 1] = polygon[b];
	board[36*(10*y+x)+facet*6 + 2] = polygon[c];
	board[36*(10*y+x)+facet*6 + 3] = polygon[b];
	board[36*(10*y+x)+facet*6 + 4] = polygon[c];
	board[36*(10*y+x)+facet*6 + 5] = polygon[d];


}

//-------------------------------------------------------------------------------------------------------------------
// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void updatetile()
{
//	cout << "in update tile" << endl;
	// Bind the VBO containing current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]); 

	vec4 newpoints[144]; 
	// For each of the 4 'cells' of the tile,
	for (int i = 0; i < 4; i++) 
	{
//		cout << "i:" << i << endl;
//		cout << "x:" << tile[i].x << endl;
//		cout << "y:" << tile[i].y << endl;
		// Calculate the grid coordinates of the cell
		GLfloat x = tilepos.x + tile[i].x - 5; 
		GLfloat y = tilepos.y + tile[i].y - 10;

		// Create the 4 corners of the square - these vertices are using location in pixels
		// These vertices are later converted by the vertex shader
		vec4 p[8] = {
			vec4(x * 33.0, (y * 33.0), 16.5, 1), 
			vec4(x * 33.0, 33.0 + (y * 33.0), 16.5, 1),
			vec4(33.0 + (x * 33.0), (y * 33.0), 16.5, 1),
			vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), 16.5, 1),
			vec4((x * 33.0), (y * 33.0), -16.5, 1), 
			vec4((x * 33.0), 33.0 + (y * 33.0), -16.5, 1),
			vec4(33.0 + (x * 33.0), (y * 33.0), -16.5, 1),
			vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), -16.5, 1),

		};
		// Two points are used by two triangles each
		addvertex(newpoints, p, 0, i, 0, 0, 1, 2, 3);
		addvertex(newpoints, p, 1, i, 0, 2, 3, 6, 7);
		addvertex(newpoints, p, 2, i, 0, 3, 1, 7, 5);
		addvertex(newpoints, p, 3, i, 0, 0, 2, 4, 6);
		addvertex(newpoints, p, 4, i, 0, 1, 0, 5, 4);
		addvertex(newpoints, p, 5, i, 0, 6, 7, 4, 5);
	}
	// Put new data in the VBO
	glBufferSubData(GL_ARRAY_BUFFER, 0, 144*sizeof(vec4), newpoints); 
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	vec4 updatecolour[144];
	for (int i=0;i<4;i++) {
		if(board[int(tilepos.x+tile[i].x)][int(tilepos.y+tile[i].y)]==true) {
			for (int j=0;j<36;j++) updatecolour[i*36 + j] = grey;
		}
		else {
			for (int j=0;j<36;j++) updatecolour[i*36 + j] = tilecolour[i];
		}
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updatecolour), updatecolour);
	glBindVertexArray(0);
	
}
//-----------------------------------------------------------------------------------------------------
//update robot arm
void updateArm() {
	if(hold) {
		vec2 head = vec2(tilepos.x-5, tilepos.y-10);
		vec2 joint = vec2(tilepos.x-10,0);
		vec2 tail = vec2(-8,-10);
		vec4 p1[8] = {
			vec4(tail.x*33.0, tail.y*33.0, 16.5, 1.0),
			vec4(tail.x*33.0, 33.0+tail.y*33.0, 16.5, 1.0),
			vec4(joint.x*33.0, joint.y*33.0, 16.5, 1.0),
			vec4(joint.x*33.0, 33.0+joint.y*33.0,16.5, 1.0),
			vec4(tail.x*33.0, tail.y*33.0, -16.5, 1.0),
			vec4(tail.x*33.0, 33.0+tail.y*33.0, -16.5, 1.0),
			vec4(joint.x*33.0, joint.y*33.0, -16.5, 1.0),
			vec4(joint.x*33.0, 33.0+joint.y*33.0,-16.5, 1.0)
		};
		vec4 lowerarm[36];
		addvertex(lowerarm,p1, 0, 0, 0, 0, 1, 2, 3);
		addvertex(lowerarm,p1, 1, 0, 0, 2, 3, 6, 7);
		addvertex(lowerarm,p1, 2, 0, 0, 3, 1, 7, 5);
		addvertex(lowerarm,p1, 3, 0, 0, 0, 2, 4, 6);
		addvertex(lowerarm,p1, 4, 0, 0, 1, 0, 5, 4);
		addvertex(lowerarm,p1, 5, 0, 0, 6, 7, 4, 5);
		
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[8]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lowerarm), lowerarm);
		glBindVertexArray(0);

		vec4 p[8] = {
			vec4(joint.x*33.0, joint.y*33.0, 16.5, 1.0),
			vec4(joint.x*33.0, 33.0+joint.y*33.0, 16.5, 1.0),
			vec4(head.x*33.0, head.y*33.0, 16.5, 1.0),
			vec4(head.x*33.0, 33.0+head.y*33.0,16.5, 1.0),
			vec4(joint.x*33.0, joint.y*33.0, -16.5, 1.0),
			vec4(joint.x*33.0, 33.0+joint.y*33.0, -16.5, 1.0),
			vec4(head.x*33.0, head.y*33.0, -16.5, 1.0),
			vec4(head.x*33.0, 33.0+head.y*33.0,-16.5, 1.0)

		};
		vec4 upperarm[36];
		addvertex(upperarm,p, 0, 0, 0, 0, 1, 2, 3);
		addvertex(upperarm,p, 1, 0, 0, 2, 3, 6, 7);
		addvertex(upperarm,p, 2, 0, 0, 3, 1, 7, 5);
		addvertex(upperarm,p, 3, 0, 0, 0, 2, 4, 6);
		addvertex(upperarm,p, 4, 0, 0, 1, 0, 5, 4);
		addvertex(upperarm,p, 5, 0, 0, 6, 7, 4, 5);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[10]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(upperarm), upperarm);
		glBindVertexArray(0);


	}
}
//-------------------------------------------------------------------------------------------------------------------

// Called at the start of play and every time a tile is placed
void newtile()
{
	tilepos = vec2(5 , 19); // Put the tile at the top of the board
	hold = true;
	state = 0;

	// Update the geometry VBO of current tile
	r_type = rand() % 6;
	for (int i = 0; i < 4; i++)
		tile[i] = allRotationsShapes[r_type][state][i]; // Get the 4 pieces of the new tile
	updatetile(); 
	updateArm();

	// Update the color VBO of current tile
	vec4 newcolours[144];
	for (int i=0;i<4;i++) {
		r_colour = rand()%5;
		switch(r_colour) {
			case 0:
				tilecolour[i] = orange;
				break;
			case 1:
				tilecolour[i] = green;
				break;
			case 2:
				tilecolour[i] = yellow;
				break;
			case 3:
				tilecolour[i] = red;
				break;
			case 4:
				tilecolour[i] = purple;
				break;
		}
		for (int j=0;j<36;j++) {
			newcolours[36*i+j] = tilecolour[i];
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

void initGrid()
{
	// ***Generate geometry data
	vec4 gridpoints[590]; // Array containing the 64 points of the 32 total lines to be later put in the VBO
	vec4 gridcolours[590]; // One colour per vertex
	// Vertical lines 
	for (int i = 0; i < 11; i++){
		gridpoints[2*i] = vec4(33.0 * i-165.0, -330, -16.5, 1);
		gridpoints[2*i + 1] = vec4(33.0 * i-165.0, 660.0-330, -16.5, 1);
		
	}
	for (int i=0; i<11; i++) {
		gridpoints[22+i*2] = vec4(33.0*i-165.0, -330.0, 16.5, 1);
		gridpoints[22+i*2 + 1] = vec4(33.0*i-165.0, 660.0-330.0, 16.5, 1);

	}
	// Horizontal lines
	for (int i = 0; i < 21; i++){
		gridpoints[44 + 2*i] = vec4(-165.0, 33.0 * i-330.0, -16.5, 1);
		gridpoints[44 + 2*i + 1] = vec4(330.0-165.0, 33.0 * i-330.0, -16.5, 1);
	}
	for (int i = 0; i < 21; i++) {
		gridpoints[86 + 2*i ] = vec4(-165, 33.0 * i-330, 16.5, 1);
		gridpoints[86 + 2*i + 1] = vec4(330.0-165, 33.0 * i-330, 16.5, 1);
	}

	//z-directional lines
	for (int i=0;i<11;i++) {
		for (int j=0;j<21;j++) {
			gridpoints[128+ 2*(21*i+j)] = vec4(33.0*i-165, 33.0*j-330, 16.5, 1);
			gridpoints[128+ 2*(21*i+j)+1] = vec4(33.0*i-165, 33.0*j-330, -16.5, 1); 
		}
	}
	// Make all grid lines white
	for (int i = 0; i < 590; i++)
		gridcolours[i] = white;


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[0]); // Bind the first VAO
	glGenBuffers(2, vboIDs); // Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]); // Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, 590*sizeof(vec4), gridpoints, GL_STATIC_DRAW); // Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPosition); // Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]); // Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, 590*sizeof(vec4), gridcolours, GL_STATIC_DRAW); // Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor); // Enable the attribute
}


void initBoard()
{
	// *** Generate the geometric data
	vec4 boardpoints[7200];
	for (int i = 0; i < 7200; i++)
		boardcolours[i] = black; // Let the empty cells on the board be black
	// Each cell is a square (2 triangles with 6 vertices)
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 10; j++)
		{		
			int x = j - 5;
			int y = i - 10;
			vec4 p[8] = {
				vec4((x * 33.0), (y * 33.0), 16.5, 1),
				vec4((x * 33.0), 33.0 + (y * 33.0), 16.5, 1),
				vec4(33.0 + (x * 33.0), (y * 33.0), 16.5, 1),
				vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), 16.5, 1),
				vec4((x * 33.0), (y * 33.0), -16.5, 1),
				vec4((x * 33.0), 33.0 + (y * 33.0), -16.5, 1),
				vec4(33.0 + (x * 33.0), (y * 33.0), -16.5, 1),
				vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), -16.5, 1) 

			};
			addvertex(boardpoints,p, 0, j, i, 0, 1, 2, 3);
			addvertex(boardpoints,p, 1, j, i, 2, 3, 6, 7);
			addvertex(boardpoints,p, 2, j, i, 3, 1, 7, 5);
			addvertex(boardpoints,p, 3, j, i, 0, 2, 4, 6);
			addvertex(boardpoints,p, 4, j, i, 1, 0, 5, 4);
			addvertex(boardpoints,p, 5, j, i, 6, 7, 4, 5);

		}
	}

	// Initially no cell is occupied
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = false; 


	// *** set up buffer objects
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 7200*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 7200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

// No geometry for current tile initially
void initCurrentTile()
{
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 144*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 144*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}
void initArm() {
	//base
	glBindVertexArray(vaoIDs[3]);
	glGenBuffers(2, &vboIDs[6]);

	// Base positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[6]);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(vec4), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vec4 p[8] = {
		vec4(-11*33, -10*33, 16.5, 1.0),
		vec4(-11*33, -9*33, 16.5, 1.0),
		vec4(-7*33, -10*33, 16.5, 1.0),
		vec4(-7*33, -9*33, 16.5,1.0),
		vec4(-11*33, -10*33, -16.5, 1.0),
		vec4(-11*33, -9*33, -16.5, 1.0),
		vec4(-7*33, -10*33, -16.5, 1.0),
		vec4(-7*33, -9*33, -16.5,1.0),
	};
	vec4 basepoints[36];
	addvertex(basepoints,p, 0, 0, 0, 0, 1, 2, 3);
	addvertex(basepoints,p, 1, 0, 0, 2, 3, 6, 7);
	addvertex(basepoints,p, 2, 0, 0, 3, 1, 7, 5);
	addvertex(basepoints,p, 3, 0, 0, 0, 2, 4, 6);
	addvertex(basepoints,p, 4, 0, 0, 1, 0, 5, 4);
	addvertex(basepoints,p, 5, 0, 0, 6, 7, 4, 5);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(basepoints), basepoints); 
	
	// Base Colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[7]);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
	vec4 basecolours[36];
	for(int i=0;i<36;i++) basecolours[i] = white;
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(basecolours), basecolours);

	// lower arm
	glBindVertexArray(vaoIDs[4]);
	glGenBuffers(2, &vboIDs[8]);

	// lower arm positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[8]);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	
	// lower arm Colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[9]);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
	vec4 lowercolour[36];
	for (int i=0;i<36;i++) lowercolour[i] = blue;
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[9]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lowercolour), lowercolour);
	glBindVertexArray(0);

	//upper arm
	glBindVertexArray(vaoIDs[5]);
	glGenBuffers(2, &vboIDs[10]);
	// upper arm positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[10]);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	
	// upper arm Colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[11]);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
	vec4 uppercolour[36];
	for (int i=0;i<36;i++) uppercolour[i] = white;
	glBindBuffer(GL_ARRAY_BUFFER,vboIDs[11]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uppercolour), uppercolour);
	glBindVertexArray(0);

}


void init()
{
	// Load shaders and use the shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Get the location of the attributes (for glVertexAttribPointer() calls)
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");

	// Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
	glGenVertexArrays(6, &vaoIDs[0]);

	// Initialize the grid, the board, and the current tile
	initGrid();
	initBoard();
	initCurrentTile();
	initArm();

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	// Game initialization
	newtile(); // create new next tile
	updateArm();

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// Rotates the current tile, if there is room
//direction may be +1 or -1, denoting the following state of the tile
void rotate(int direction)
{
    int i;
    GLfloat x,y;
    for(i=0;i<4;i++) {
	x = tilepos.x + allRotationsShapes[r_type][(state+direction)%4][i].x;
	y = tilepos.y + allRotationsShapes[r_type][(state+direction)%4][i].y;
	if((board[int(x)][int(y)]==true&&!hold)||
	   x<0||
	   x>=10||
	   y<0||
	   y>=20) break;
    }
    if(i==4) {
      state = (state+direction)%4;
//      cout << state << endl;
      for(int j=0;j<4;j++) {
	tile[j].x =  allRotationsShapes[r_type][state][j].x;
	tile[j].y =  allRotationsShapes[r_type][state][j].y;
      }
    }
    updatetile();
    updateArm();
}

//-------------------------------------------------------------------------------------------------------------------

// Checks if the specified row (0 is the bottom 19 the top) is full
// If every cell in the row is occupied, it will clear that cell and everything above it will shift down one row
void checkfullrow(int row)
{
//    cout << "row:" <<row<<endl;
    int i;
    for(i=0;i<10;i++) if(!board[i][row]) break;
    if(i==10) {
		linecounter ++;
//        cout<< "full!"<<endl;
        for(int j=0;j<10;j++) removetable[j][row]=true;
//        for(int j=0;j<10;j++) {
//            for(int k=row; k<19;k++) {
//                board[j][k] = board[j][k+1];
//				for (int l=0;l<6;l++) boardcolours[6*(10*k+j)+l]=boardcolours[6*(10*(k+1)+j)+l];
//            }
//            board[j][19] = false;
//			for(int l=0;l<6;l++) boardcolours[6*(10*19+j)+l] = black;
//        }
    }

}
//-------------------------------------------------------------------------------------------------------------------
bool comparecolour(vec4& a, vec4& b) {
	if(a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w) return true;
	else return false;

}
bool checkcolour(int x, int y) {
	if(comparecolour(boardcolours[36*(10*y+x)],black)) return false;
	int d_up=0, d_down=0;
	int d_left = 0, d_right=0;
	int d_dia_lu = 0, d_dia_rd=0;
	int d_dia_ld = 0, d_dia_ru=0;
	// up and down directions
	while(y+d_up+1<=19&&comparecolour(boardcolours[36*(10*(y+d_up+1)+x)],boardcolours[36*(10*y+x)])) d_up++;
	while(y-d_down-1>=0&&comparecolour(boardcolours[36*(10*(y-d_down-1)+x)],boardcolours[36*(10*y+x)])) d_down++;

	//left and right directions
	while(x+d_right+1<=9 && comparecolour(boardcolours[36*(10*y+x+d_right+1)],boardcolours[36*(10*y+x)])) d_right++;
	while(x-d_left-1>=0 && comparecolour(boardcolours[36*(10*y+x-d_left-1)],boardcolours[36*(10*y+x)])) d_left++;

	//left up to right down diagonal
	while(x-d_dia_lu-1>=0&&y+d_dia_lu+1<=19&&comparecolour(boardcolours[36*(10*(y+d_dia_lu+1)+(x-d_dia_lu-1))],boardcolours[36*(10*y+x)])) d_dia_lu++;
	while(x+d_dia_rd+1<=9&&y-d_dia_rd-1>=0&&comparecolour(boardcolours[36*(10*(y-d_dia_rd-1)+(x+d_dia_rd+1))],boardcolours[36*(10*y+x)])) d_dia_rd++;

	//left down to right up diagonal
	while(x-d_dia_ld-1>=0 && y-d_dia_ld-1>=0 && comparecolour(boardcolours[36*(10*(y-d_dia_ld-1)+(x-d_dia_ld-1))],boardcolours[36*(10*y+x)])) d_dia_ld++;
	while(x+d_dia_ru+1<=9 && y+d_dia_ru+1<=19 && comparecolour(boardcolours[36*(10*(y+d_dia_ru+1)+(x+d_dia_ru+1))],boardcolours[36*(10*y+x)])) d_dia_ru++;
	cout << "x:" << x << endl;
	cout << "y:" << y << endl;
	if(d_up+d_down<2&&d_left+d_right<2&&d_dia_lu+d_dia_rd<2&&d_dia_ld+d_dia_ru<2) return false;
	else {
		removetable[x][y] = true;
		if(d_up+d_down>=2) {
			cout << "up and down:"<< d_up << ";" << d_down << endl;
			for (int i=1;i<=d_up;i++) removetable[x][y+i] = true;
			for (int i=1;i<=d_down;i++) removetable[x][y-i] = true;
		}
		if(d_left+d_right>=2) {
			cout << "left and right:"<< d_left << ";" << d_right << endl;
			for (int i=1;i<=d_left;i++) removetable[x-i][y] = true;
			for (int i=1;i<=d_right;i++) removetable[x+i][y] = true;

		}
		if(d_dia_lu+d_dia_rd>=2) {
			cout << "left_up and right_down:"<< d_dia_lu << ";" << d_dia_rd << endl;
			for (int i=1;i<=d_dia_lu;i++) removetable[x-i][y+i] = true;
			for (int i=1;i<=d_dia_rd;i++) removetable[x+i][y-i] = true;

		}
		if(d_dia_ru+d_dia_ld>=2) {
			cout << "right up and left down:"<< d_dia_ru << ";" << d_dia_ld << endl;
			for (int i=1;i<=d_dia_ru;i++) removetable[x+i][y+i] = true;
			for (int i=1;i<=d_dia_ld;i++) removetable[x-i][y-i] = true;

		}
	}
	return true;
}
//-------------------------------------------------------------------------------------------------------------------

// Starts the game over - empties the board, creates new tiles, resets line counters
void restart()
{

	linecounter = 0;
	for(int i=0;i<10;i++) {
		for(int j=0;j<20;j++) {
			board[i][j] = false;
		}
	}
	for(int j=0;j<7200;j++) {
		boardcolours[j] = black;
	}
	updateBoard();
	
	newtile();
	updateArm();
}
//-------------------------------------------------------------------------------------------------------------------
void clearremovetable() {
	for (int i=0;i<10;i++) {
		for (int j=0;j<20;j++) {
			removetable[i][j] = false;
		}
	}
}
void moveboard(int from_x, int from_y, int to_x, int to_y) {
	if(to_x<=9&&to_x>=0&&to_y<=19&&to_y>=0) {
		if(from_x<=9&&from_x>=0&&from_y<=19&&from_y>=0) {
			for (int k=0;k<36;k++) {
				boardcolours[36*(10*(to_y)+to_x)+k] = boardcolours[36*(10*(from_y)+from_x)+k];
			}
			board[to_x][to_y] = board[from_x][from_y];
		}
		else {
			for (int k=0;k<36;k++) {
				boardcolours[36*(10*to_y+to_x)+k] = black;
			}
			board[to_x][to_y] = false;
		}
	}

}
void executeremovetable() {
	for(int i=0;i<10;i++) {
		int count = 0;
		for (int j=0;j<20;j++) {
			if(removetable[i][j]==true) count ++;
			else {
				moveboard(i,j,i,j-count);
			}
		}
		for (int j=count;j>0;j--) {
			for (int k=0;k<36;k++) {
				boardcolours[36*(10*(20-j)+i)+k] = black;
			}
			board[i][20-j] = false;
		}
	}
	clearremovetable();
	int flag=0;
	for(int i=0;i<10;i++) {
		for(int j=0;j<20;j++) {
			if(checkcolour(i,j)==true) flag ++;
		}
	}
	if(flag!=0) executeremovetable();
}
//-------------------------------------------------------------------------------------------------------------------
// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void settle()
{
	for (int i=0;i<4;i++) {
        board[int(tilepos.x+tile[i].x)][int(tilepos.y+tile[i].y)] = true;
//		cout << tilecolour[i] << endl;
		for (int j=0;j<36;j++) {
			boardcolours[36 * (10 * int(tilepos.y + tile[i].y) + int(tilepos.x + tile[i].x)) + j] = tilecolour[i];
		}
    }
	cout << "settled:update Board" <<endl;
	updateBoard();
	cout << "clear removetable" << endl;
	clearremovetable();
    for (int i=0;i<4;i++) {
        checkfullrow(tilepos.y+tile[i].y);
    }
	for (int i=0;i<4;i++) {
		for (int j=0;j<10;j++) {
			checkcolour(j,tilepos.y+tile[i].y);
		}
	}
	executeremovetable();
	clearremovetable();
	updateBoard();
	int i;
	for (i=0;i<10;i++) {
		if(board[i][19]) break;
	}
	if(i==10) {
		newtile();
		updateArm();
	}
	else restart(); //losed, restart 
} 
//------------------------------------------------------------------------------------------------------------------- 
// Given (x,y), tries to move the tile x squares to the right and y squares down // Returns true if the tile was successfully moved, or false if there was some issue 
bool movetile(vec2 direction) {
	GLfloat x,y; 
	int i; 
	for(i=0;i<4;i++) { 
		x = tile[i].x + tilepos.x + direction.x; 
		y = tile[i].y + tilepos.y + direction.y; 
		if((board[int(x)][int(y)]==true&&!hold)||y<0||x<0||x>=10||y>=20) break;
	 } 
	 if(i==4) { 
		tilepos.x += direction.x; 
		tilepos.y += direction.y; 
		updatetile(); 
		updateArm();
	}else if(direction.x==0 && direction.y==-1&&!hold) { 
		settle();
    	}
	return false;
}

//-------------------------------------------------------------------------------------------------------------------

// Draws the game
void display()
{

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUniform1i(locxsize, xsize); // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
	glUniform1i(locysize, ysize);
	// set up viewing transformation
    	//vec4  eye( radius*sin(theta)*cos(phi),
	//	 radius * sin(theta) * sin(phi),
	//	 radius * cos(theta),
	//	 1.0 );
	vec4 eye(radius*sin(theta), 10, radius*cos(theta),1.0);
	//vec4  eye( 0.0, 0.0, 2.0,1.0 );
	//vec4 	eye(0.0,0.0,2.0,1.0);
    	vec4  	at( 0.0, 0.0, 0.0, 1.0 );
    	vec4    up = vec4(-sin(theta), 1.0, -cos(theta), 0.0);

    	mat4  mv = LookAt( eye, at, up );
    	glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );

    	//set up projection transformation
    	mat4  p = Ortho( b_left, b_right, b_bottom, b_top, zNear, zFar );
    	glUniformMatrix4fv( projection, 1, GL_TRUE, p );

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
	glDrawArrays(GL_TRIANGLES, 0, 7200); // Draw the board (10*20*2 = 400 triangles)

	glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 144); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, 590); // Draw the grid lines (21+11 = 32 lines)
	
	glBindVertexArray(vaoIDs[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(vaoIDs[4]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(vaoIDs[5]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------------------------------

// Reshape callback will simply change xsize and ysize variables, which are passed to the vertex shader
// to keep the game the same from stretching if the window is stretched
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

//-------------------------------------------------------------------------------------------------------------------

// Handle arrow key keypresses
void special(int key, int x, int y)
{
    switch(key) {
        case GLUT_KEY_LEFT:
	    movetile(vec2(-1,0));
	    break;
	case GLUT_KEY_RIGHT:
	    movetile(vec2(1,0));
	    break;
	case GLUT_KEY_DOWN:
	    movetile(vec2(0,-1));
	    break;
	case GLUT_KEY_UP:
	    rotate(1);
	    break;
    }
}

//-------------------------------------------------------------------------------------------------------------------

// Handles standard keypresses
void keyboard(unsigned char key, int x, int y)
{
	int i;
	switch(key) 
	{
		case 033: // Both escape key and 'q' cause the game to exit
		    exit(EXIT_SUCCESS);
		    break;
		case 32: //drop the tile
		    for(i=0;i<4;i++) {
			GLfloat x = tile[i].x + tilepos.x; 
			GLfloat y = tile[i].y + tilepos.y; 
			if(board[int(x)][int(y)]==true) break;
		    }
		    if(i==4) hold = false;
		    break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		//case 'r': // 'r' key restarts the game
		//	restart();
		//	break;
		case 'x': b_left *= 1.1; b_right *= 1.1; break;
		case 'X': b_left *= 0.9; b_right *= 0.9; break;
		case 'y': b_bottom *= 1.1; b_top *= 1.1; break;
		case 'Y': b_bottom *= 0.9; b_top *= 0.9; break;
		case 'z': zNear  *= 1.1; zFar *= 1.1; break;
		case 'Z': zNear *= 0.9; zFar *= 0.9; break;
		case 'r': radius *= 2.0; break;
		case 'R': radius *= 0.5; break;
		case 'o': theta += dr; break;
		case 'O': theta -= dr; break;
		case 'p': phi += dr; break;
		case 'P': phi -= dr; break;

	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

void idle(void)
{
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

void timer(int value) {
	if(!hold) {
		movetile(vec2(0,-1));
		updatetile();
	}
	glutTimerFunc(100,timer,10);
}
//------------------------------------------------------------------------------------------------------------------
void timer2(int value) {
	if(hold) {
		armcount ++;
		if(armcount >= 5) {
			armcount = 0;
			int i=0;
			for (i=0;i<4;i++) {
				if(board[int(tilepos.x+tile[i].x)][int(tilepos.y+tile[i].y)])  break;
			}
			if(i==4) hold=false;
			else restart();
		}
	}
	else {
		armcount = 0;
	}
	glutTimerFunc(1000,timer2,10);
}

//-------------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(680, 200); // Center the game window (well, on a 1920x1080 display)
	glutCreateWindow("Fruit Tetris");
	glewInit();
	init();

	// Callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutTimerFunc(300, timer, 10);
	glutTimerFunc(1000, timer2, 10);

	glutMainLoop(); // Start main loop
	return 0;
}
