#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/vec.h"


struct triangle {
	int index;
	vec3 vertex[3];

	vec3 mat_ambient;    // material property used in Phong model
	vec3 mat_diffuse;
	vec3 mat_specular;

	float mat_shineness;
	float reflectance;
	float refr;
	float refractivity;
  
	struct triangle *next;
};
float intersect_triangle(vec3 origin, vec3 direction, struct triangle* tri, vec3* hit);
void *intersect_scene(vec3 origin, vec3 direction, struct triangle* list, vec3* hit, int* isplane);
vec3 triangle_normal(struct triangle* tri, vec3 point);
struct triangle* add_triangle(struct triangle * list, int index, vec3 v1, vec3 v2, vec3 v3, vec3 amb, vec3 diff, vec3 spec, float shine, float refl, float refr, float refractivity);

struct board { //the infinitely large board
	float height;
	float reflectance;
	
	vec3 mat_ambient;
	vec3 mat_diffuse1; //white color
	vec3 mat_diffuse2; //black color
	vec3 mat_specular;
	float shineness;
};
float intersect_plane(vec3 origin, vec3 direction, vec3 *hit);
const float precision = 0.000001; //to determine floating number equality
const float t_precise = 0.001; //determine t value in intersection to avoid self intersection

