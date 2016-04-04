//
// this provide functions to set up the scene
//
//#include "sphere.h"
#include <stdio.h>
#include "chess.h"
//#include "object.h"
extern vec3 light1;
extern vec3 light1_intensity;
extern float light1_shadow;

extern vec3 global_ambient;
extern triangle* scene;

extern vec3 background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

/**************************************
set up the chess pieces
**************************************/
float *Vertices = NULL;
unsigned int *Indices = NULL;

int VertexNum;
int FaceNum;

void readSMF(const char *name){
    FILE *fin = NULL;
    fin = fopen(name,"r");
    if(fin == NULL){
        printf("Error Opening the File\n");
        fclose(fin); exit(1);
    }

    if( fscanf(fin," # %d %d",&VertexNum,&FaceNum) < 2) {
        printf("Error Reading the File\n");
        fclose(fin); exit(1);
    }

    if(Vertices != NULL) delete Vertices;
    Vertices = new float [ VertexNum * 3 ];
    if(Vertices == NULL) {
        printf("Error Allocating Memory\n");
        fclose(fin); exit(1);
    }

    if(Indices != NULL) delete Indices;
    Indices = new unsigned int [ FaceNum * 3 ];
    if(Indices == NULL) {
        printf("Error Allocating Memory\n");
        fclose(fin); exit(1);
    }

    for(int i=0;i<VertexNum;i++)
        if( fscanf(fin," v %f %f %f",&Vertices[i*3], &Vertices[i*3 +1], &Vertices[i*3 +2]) < 3){
            printf("Error Reading the File\n");
            fclose(fin); exit(1);
        }

    int ta,tb,tc;
    for(int i=0;i<FaceNum;i++){
        if( fscanf(fin," f %d %d %d",&ta,&tb,&tc) < 3){
            printf("Error Reading the File\n");
            fclose(fin); exit(1);
        }
        Indices[i*3] = ta-1;
        Indices[i*3 +1] = tb-1;
        Indices[i*3 +2] = tc-1;
    }

    fclose(fin);
}

void set_up_chess() {

    background_clr.x = 0.5;
    background_clr.y = 0.05;
    background_clr.z = 0.8;
    // set up global ambient term
    global_ambient.x = 0.2;
    global_ambient.y = 0.2;
    global_ambient.z = 0.2;

    // set up light 1
    light1.x = -2.0;
    light1.y = 5.0;
    light1.z = 1.0;
    light1_intensity.x = 1.0;
    light1_intensity.y = 1.0;
    light1_intensity.z = 1.0;
    light1_shadow = 0;

    // set up decay parameters
    decay_a = 0.5;
    decay_b = 0.3;
    decay_c = 0.0;

    const char pieceName[] = "chess_pieces/chess_piece.smf";    

    readSMF(pieceName);
    //checkMinMax();

    vec3 mat_ambient = vec3(0.7, 0.5, 0.5);
    vec3 mat_diffuse = vec3(0.1, 0.3, 0.5);
    vec3 mat_specular = vec3(1.0, 1.0, 1.0);
    float  mat_shineness = 40;

    float reflectance = 0.6;

    float refractivity = 1.5;
    float refractance = 0.2;

    vec3 point[3];

    for(int i=0;i<FaceNum;i++){
        for(int j=0;j<3;j++){
            int pNum = Indices[ i*3 + j];
            point[j] = vec3( Vertices[ pNum*3 ], Vertices[ pNum*3 +1]-0.75, Vertices[ pNum*3 +2]-2);
        }
	scene = add_triangle(scene, i, point[0], point[1], point[2], mat_ambient, mat_diffuse, mat_specular,  mat_shineness, reflectance, refractance, refractivity);
    }
//    delete [] Vertices; Vertices = NULL;
 //   delete [] Indices; Indices = NULL;

    const char piece[] = "chess_pieces/chess_piece.smf";
    readSMF(piece);

    mat_ambient = vec3(0.6, 0.6, 0.6);
    mat_diffuse = vec3(0.7, 0.0, 0.25);
    mat_specular = vec3(1.0, 1.0, 1.0);
    mat_shineness = 40;

    reflectance = 0.2;
    refractance = 0.6;

    refractance = 0.6;


    for(int i=0;i<FaceNum;i++){
        for(int j=0;j<3;j++){
            int pNum = Indices[ i*3 + j];
            point[j] = vec3( Vertices[ pNum*3 ]  , Vertices[ pNum*3 +1], Vertices[ pNum*3 +2] -2.5 );
        }
	scene = add_triangle(scene, i+FaceNum, point[0], point[1], point[2], mat_ambient, mat_diffuse, mat_specular,  mat_shineness, reflectance, refractance, refractivity);
    }

    printf("---Read chessPiece OK---\n");

    delete [] Vertices; Vertices = NULL;
    delete [] Indices; Indices = NULL;

}
