/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
#include"include/vec.h"

typedef struct sphere {
  int index;               // identifies a sphere; must be greater than 0

  vec3 center;
  float radius;

  vec3 mat_ambient;    // material property used in Phong model
  vec3 mat_diffuse;
  vec3 mat_specular;
  float mat_shineness;

  float reflectance;       // this number [0,1] determines how much 
                           // reflected light contributes to the color
                           // of a pixel
  struct sphere *next;
} Spheres;   // a list of spheres

// intersect ray with sphere
void *intersect_scene(vec3, vec3, Spheres *, vec3 *, int *);
// return the unit normal at a point on sphere
vec3 sphere_normal(vec3, Spheres *);
// add a sphere to the sphere list
Spheres *add_sphere(Spheres *, vec3, float, vec3 , vec3, vec3, float, float, int);

struct plane {
	vec3 leftbottom;
	vec3 righttop;
	float reflectance;
	
	vec3 mat_ambient;
	vec3 mat_diffuse1;
	vec3 mat_diffuse2;
	vec3 mat_specular;
	float shineness;
};

float intersect_plane(vec3, vec3, vec3*);
