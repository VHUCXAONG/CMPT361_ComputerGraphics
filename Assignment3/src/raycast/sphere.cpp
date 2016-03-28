#include "include/vec.h"
#include "sphere.h"
#include <stdlib.h>
#include <math.h>

/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
extern struct plane pl;
extern int board_on;
float intersect_plane(vec3 o, vec3 u, vec3 *hit) {
	if(u.y==0) return -1.0;
	
	float t = (pl.leftbottom.y-o.y)/u.y;
	hit->x = o.x + t*u.x;
	hit->y = o.y + t*u.y;
	hit->z = o.z + t*u.z;
	if(hit->x>pl.righttop.x || hit->x<pl.leftbottom.x || hit->z<pl.righttop.z || hit->z>pl.leftbottom.z) return -1.0;
	
	return t;
}
float intersect_sphere(vec3 o, vec3 u, Spheres *sph, vec3 *hit) {
	float x1 = o.x - sph->center.x;
	float y1 = o.y - sph->center.y;
	float z1 = o.z - sph->center.z;

	float a = dot(u,u);

	float b = 2.0*(u.x*x1 + u.y*y1 + u.z*z1);

	float c = x1*x1 + y1*y1 + z1*z1 - (sph->radius) * (sph->radius);
	
	float delta = b*b - 4*a*c;
	if(delta<-precision) {
		//printf("no root");
		return -1.0;	
	}
	 
	float t1 = (-b + sqrt(delta)) / (2*a);
	float t2 = (-b - sqrt(delta)) / (2*a);
	//printf("root1:%f\n",t1);
	//printf("root2:%f\n",t2);
	hit->x = t2*u.x+ o.x;
	hit->y = t2*u.y+ o.y;
	hit->z = t2*u.z+ o.z;
	return t2;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
void *intersect_scene(vec3 s, vec3 ray, Spheres *slist, vec3 *hit, int *isplane) {
//
// do your thing here
//
	//printf("intersecting the scene...\n");
	Spheres* l = slist;
	void* rv = NULL;
	//vec3 oldhit;
	vec3 nhit;
	bool newhit = true;
	float itsct = -1.0;
	float oldsct = -1.0;
	

	if(board_on) {
		itsct = intersect_plane(s, ray, &nhit);
		if(itsct > 0 && fabs(itsct)>t_precise) {
			oldsct = itsct;
			newhit = false;
			rv = &pl;
			hit->x = nhit.x;
			hit->y = nhit.y;
			hit->z = nhit.z;
			*isplane = 1;
		}
	}
	while(l) {
	//	printf("sphere address:%u\n",l);
	//	printf("%f %f %f\n",l->center.x, l->center.y, l->center.z);
	//	printf("%f\n",l->radius);
		itsct = intersect_sphere(s, ray, l, &nhit);
		//printf("intersect value:%f\n", itsct);
		if(itsct>0 && fabs(itsct) > t_precise) {
	//		printf("itsect:%f\n",itsct);
			if(newhit==true || ((newhit==false)&&(itsct<oldsct))) {
				oldsct = itsct;
				newhit = false;
				rv = l;
				hit->x = nhit.x;
				hit->y = nhit.y;
				hit->z = nhit.z;
				*isplane = 0;
			}
		}
		l = l->next;
	}
	return rv;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, vec3 ctr, float rad, vec3 amb,
		    vec3 dif, vec3 spe, float shine, 
		    float refl, float refr, float refractivity, int sindex) {
  Spheres *new_sphere;

  new_sphere = (Spheres *)malloc(sizeof(Spheres));
  new_sphere->index = sindex;
  new_sphere->center = ctr;
  new_sphere->radius = rad;
  (new_sphere->mat_ambient).x = amb.x;
  (new_sphere->mat_ambient).y = amb.y;
  (new_sphere->mat_ambient).z = amb.z;
  (new_sphere->mat_diffuse).x = dif.x;
  (new_sphere->mat_diffuse).y = dif.y;
  (new_sphere->mat_diffuse).z = dif.z;
  (new_sphere->mat_specular).x = spe.x;
  (new_sphere->mat_specular).y = spe.y;
  (new_sphere->mat_specular).z = spe.z;
  new_sphere->mat_shineness = shine;
  new_sphere->reflectance = refl;
  new_sphere->refr = refr;
  new_sphere->refractivity = refractivity;
  new_sphere->next = NULL;

  if (slist == NULL) { // first object
    slist = new_sphere;
  } else { // insert at the beginning
    new_sphere->next = slist;
    slist = new_sphere;
  }

  return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
vec3 sphere_normal(vec3 q, Spheres *sph) {
  vec3 rc;
  rc = q - sph->center;
  rc = normalize(rc);
  return rc;
}
