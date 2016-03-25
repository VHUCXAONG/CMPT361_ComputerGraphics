#include"include/vec.h"
class Object {
public:
	int index;

	int type; // is it a sphere or a plane

	vec3 mat_ambient;    // material property used in Phong model
	vec3 mat_diffuse;
	vec3 mat_specular;
	float mat_shineness;

	float reflectance;       // this number [0,1] determines how much 
				   // reflected light contributes to the color
				   // of a pixel
	Object * next;
	
	Object(int id, int t, vec3 amb, vec3 diff, vec3 spec, float shine, float relec):
	index(id), type(t), mat_ambient(amb), mat_diffuse(diff), mat_specular(spec), mat_shineness(shine), reflectance(reflec), next(NULL){}
	
	virtual float intersect(vec3 origin, vec3 dir, vec3* hit) = 0;
	virtual vec3 get_surfnormal(vec3 point) = 0;
	
};

class sphere: public Object {
public:
	vec3 center;
	float radius;
	
	sphere(int id, int t, vec3 amb, vec3 diff, vec3 spec, float shine, float relec, float r, vec3 c):
	Object(id, t, amb, diff, spec, shine, reflec), radius(r), center(c) {};
	
	float intersect(vec3 origin, vec3 dir, vec3* hit);
	vec3 get_surfnormal(vec3 point);
	
};

class plane: public Object {
public:
	vec3 lowerbottom;
	vec3 uppertop;
	
	plane(int id, int t, vec3 amb, vec3 diff, vec3 spec, float shine, float relec, vec3 lb, vec3 ut):
	Object(id, t, amb, diff, spec, shine, reflec), lowerbottom(lb), uppertop(ut) {};

	float intersect(vec3 origin, vec3 dir, vec3* hit);
	vec3 get_surfnormal(vec3 point);
	
	
};



