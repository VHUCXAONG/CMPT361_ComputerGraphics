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
float intersect_sphere(vec3 o, vec3 u, Spheres *sph, vec3 *hit) {
	float rsquare = sph->radius * sph->radius;
	float a = u.x*u.x + u.y*u.y + u.z*u.z;
	float b = 2*(o.x*u.x + o.y*u.y + o.z*u.z - sph->center.x*u.x - sph->center.y*u.y - sph->center.z*u.z);
	float c = sph->center.x*sph->center.x + sph->center.y*sph->center.y + sph->center.z*sph->center.z - 2*sph->center.x*o.x - 2*sph->center.y*o.y - 2*sph->center.z*o.z- rsquare;
	
	float delta = b*b - 4*a*c;
	if(delta<0) {
		//printf("no root");
		return -1.0;	
	}
	 
	float t1 = (-b + sqrt(delta)) / (2*a);
	float t2 = (-b - sqrt(delta)) / (2*a);
	//printf("root1:%f\n",t1);
	//printf("root2:%f\n",t2);
	
	vec3 scale1 = u*t1;
	vec3 scale2 = u*t2;

	vec3 intersect1;
	intersect1.x = o.x + scale1.x;
	intersect1.y = o.y + scale1.y;
	intersect1.z = o.z + scale1.z;

	vec3 intersect2;
	intersect2.x = o.x + scale2.x;
	intersect2.y = o.y + scale2.y;
	intersect2.z = o.z + scale2.z;

	float dist1 = dot(intersect1, intersect1);
	float dist2 = dot(intersect2, intersect2);
	
	if(dist1<=dist2) {
		hit->x = intersect1.x;
		hit->y = intersect1.y;
		hit->z = intersect1.z;
		return t1;
	}
	else {
		hit->x = intersect2.x;
		hit->y = intersect2.y;
		hit->z = intersect2.z;
		return t2;
	}
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(vec3 s, vec3 ray, Spheres *slist, vec3 *hit) {
//
// do your thing here
//
	//printf("intersecting the scene...\n");
	Spheres* l = slist;
	Spheres* rv = NULL;
	vec3 oldhit;
	bool newhit = true;
	float itsct = -1.0;
	
	while(l) {
		//printf("sphere address:%u\n",l);
		//printf("%f %f %f\n",l->center.x, l->center.y, l->center.z);
		//printf("%f\n",l->radius);
		itsct = intersect_sphere(s, ray, l, hit);
		//printf("intersect value:%f\n", itsct);
		if(itsct!=-1.0) {
			if(newhit==true || ((newhit==false)&&(hit->x*hit->x+hit->y*hit->y+hit->z*hit->z) < (oldhit.x*oldhit.x+oldhit.y*oldhit.y+oldhit.z*oldhit.z))) {
				newhit = false;
				rv = l;
				oldhit.x = hit->x;
				oldhit.y = hit->y;
				oldhit.z = hit->z;
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
		    float refl, int sindex) {
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
