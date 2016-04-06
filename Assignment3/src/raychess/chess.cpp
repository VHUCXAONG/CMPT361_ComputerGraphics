#include "chess.h"
extern struct board pl;
extern int board_on;
float intersect_triangle(vec3 origin, vec3 direction, struct triangle* tri, vec3* hit) {
	vec3 triangle_normal = normalize(cross(tri->vertex[1]-tri->vertex[0], tri->vertex[2]-tri->vertex[1]));
	float dividend = dot(tri->vertex[0]-origin, triangle_normal);
	float divisor = dot(direction, triangle_normal);
	//printf("dividend:%f\n",dividend);
	//printf("divisor:%f\n", divisor);
	if(divisor>-precision && divisor<precision) {
	//	printf("divisor almost 0\n");
		return -1.0;
	}
	
	float dist = dividend / divisor;
	//printf("distance:%f\n",dist);
	if(dist<-precision) {
	//	printf("distance zero\n");
		return -1.0;
	}
	
	vec3 point = dist*direction + origin;
	bool inside = true;
	for(int i=0;i<3;i++) {
		vec3 edge = tri->vertex[(i+1)%3] - tri->vertex[i];
		vec3 line = point - tri->vertex[i];
		
		if(dot(triangle_normal, cross(edge, line)) < -precision) {
			inside = false;
			break;
		}
	}
	if(!inside) {
	//	printf("not inside\n");
		return -1.0;
	}
	hit->x = point.x;
	hit->y = point.y;
	hit->z = point.z;
	//printf("intersect\n");
	return dist;
}

float intersect_plane(vec3 o, vec3 u, vec3 *hit) {
	if(u.y==0) return -1.0;
	
	float t = (pl.height-o.y)/u.y;
	hit->x = o.x + t*u.x;
	hit->y = o.y + t*u.y;
	hit->z = o.z + t*u.z;
	
	return t;
}

void *intersect_scene(vec3 origin, vec3 direction, struct triangle* list, vec3* hit, int *isplane) {
	struct triangle* l = list;
	void* rv = NULL;
	vec3 nhit;
	bool newhit = true;
	float itsct = -1.0;
	float oldsct = -1.0;
	

	if(board_on) {
		itsct = intersect_plane(origin, direction, &nhit);
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
		itsct = intersect_triangle(origin, direction, l, &nhit);
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
vec3 triangle_normal(struct triangle* tri, vec3 point) {
	vec3 vector1 = tri->vertex[0] - tri->vertex[1];
	vec3 vector2 = tri->vertex[2] - tri->vertex[1];
	
	vec3 tri_norm = cross(vector2, vector1);
	return normalize(tri_norm);
}
struct triangle* add_triangle(struct triangle * list, int index, vec3 v1, vec3 v2, vec3 v3, vec3 amb, vec3 diff, vec3 spec, float shine, float refl, float refr, float refractivity) {
	struct triangle *tri = (struct triangle *)malloc(sizeof(struct triangle));
	
	tri->index = index;

	tri->vertex[0] = v1;
	tri->vertex[1] = v2;
	tri->vertex[2] = v3;

	tri->mat_ambient  = amb;   // material property used in Phong model
	tri->mat_diffuse = diff;
	tri->mat_specular = spec;

	tri->mat_shineness = shine;
	tri->reflectance = refl;
	tri->refr = refr;
	tri->refractivity=refractivity;
	
	tri->next = list;
	
	return tri;
}
